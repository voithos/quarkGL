// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  constexpr int width = 800, height = 600;

  qrk::Window win(width, height, "Bloom", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 2.5f));
  camera->setSpeed(10.0f);
  camera->lookAt(glm::vec3(0.0f, 0.0f, 10.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/bloom.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  glm::vec3 ambient(0.0f);
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(0.0f, 0.0f, 49.5f), ambient,
      /*diffuse=*/glm::vec3(200.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-1.4f, -1.9f, 9.0f), ambient,
      /*diffuse=*/glm::vec3(0.1f, 0.0f, 0.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(0.0f, -1.8f, 4.0f), ambient,
      /*diffuse=*/glm::vec3(0.0f, 0.0f, 0.2f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(0.8f, -1.7f, 6.0f), ambient,
      /*diffuse=*/glm::vec3(0.0f, 0.1f, 0.0f)));

  qrk::Attenuation attenuation = {
      .constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
  for (auto& light : lights) {
    light->setAmbient(glm::vec3(0.0f));
    light->setSpecular(glm::vec3(0.0f));
    light->setAttenuation(attenuation);
    lightRegistry->addLight(light);
  }

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 1.032f);

  // Setup the tunnel.
  qrk::CubeMesh tunnel("examples/assets/wood.png");
  tunnel.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 25.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(5.0f, 5.0f, 55.0f)));

  // Framebuffer.
  qrk::Framebuffer mainFb(win.getSize());
  auto colorAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  auto bloomAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  mainFb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  // TODO: It'd be nice if we could just swap out the textures instead of
  // creating separate meshes.
  qrk::ScreenQuadMesh screenQuad(colorAttachment.asTexture());
  qrk::ScreenQuadMesh bloomQuad(bloomAttachment.asTexture());

  // Create ping-pong buffers for blurring.
  qrk::Framebuffer blurFb1(win.getSize());
  qrk::Framebuffer blurFb2(win.getSize());
  auto blurAttachment1 =
      blurFb1.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  qrk::ScreenQuadMesh blurQuad1(blurAttachment1.asTexture());
  qrk::ScreenQuadMesh blurQuad2(
      blurFb2.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA).asTexture());

  qrk::ScreenQuadShader blurShader(
      qrk::ShaderPath("examples/shaders/bloom_blur.frag"));
  qrk::ScreenQuadShader screenShader(
      qrk::ShaderPath("examples/shaders/hdr.frag"));
  qrk::ScreenQuadShader bloomScreenShader(
      qrk::ShaderPath("examples/shaders/bloom_screen.frag"));

  bool useBloom = true;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) { useBloom = !useBloom; });
  int drawOption = 0;
  constexpr int NUM_DRAW_OPTIONS = 3;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) {
    drawOption = (drawOption + 1) % NUM_DRAW_OPTIONS;
  });

  // win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // Draw the scene to the HDR framebuffer.
    mainFb.activate();
    mainFb.clear();

    mainShader.updateUniforms();
    mainShader.setBool("inverseNormals", true);
    mainShader.setBool("skipGamma", true);
    tunnel.draw(mainShader);

    mainFb.deactivate();

    win.setViewport();
    screenShader.updateUniforms();

    if (drawOption == 1) {
      bloomQuad.draw(screenShader);
      return;
    }

    // Ping-pong between horizontal and vertical gaussian blurs.
    // TODO: Extract this to a helper?
    qrk::Framebuffer* pingPongFbs[2] = {&blurFb1, &blurFb2};
    qrk::ScreenQuadMesh* pingPongMeshes[2] = {&blurQuad1, &blurQuad2};
    bool horizontal = false;
    auto* pingPongFb = pingPongFbs[horizontal];
    auto* pingPongMesh = &bloomQuad;

    // 5 horizontal, 5 vertical.
    constexpr int totalIterations = 10;
    for (int i = 0; i < totalIterations; i++) {
      pingPongFb->activate();

      blurShader.setBool("horizontal", horizontal);
      pingPongMesh->draw(blurShader);

      horizontal = !horizontal;
      // For a given 'horizontal' setting, we render _from_ the other mesh
      // (non-horizontal) to the corresponding framebuffer.
      pingPongFb = pingPongFbs[horizontal];
      pingPongMesh = pingPongMeshes[!horizontal];

      pingPongFb->deactivate();
    }

    qrk::ScreenQuadMesh* blurredMesh = pingPongMesh;

    if (drawOption == 2) {
      blurredMesh->draw(screenShader);
      return;
    }

    // drawOption == 0
    qrk::Texture bloomTexture = blurAttachment1.asTexture();
    bloomTexture.bindToUnit(1);
    bloomScreenShader.setBool("useBloom", useBloom);
    bloomScreenShader.setInt("bloomTexture", 1);
    screenQuad.draw(bloomScreenShader);
  });

  return 0;
}
