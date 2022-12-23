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
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 5.0f));
  camera->lookAt(glm::vec3(0.0f, 0.0f, 10.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  cameraControls->setSpeed(10.0f);
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/bloom.frag"));
  mainShader.addUniformSource(camera);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/bloom_lamp.frag"));
  lampShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(0.0f, 0.5f, 1.5f),
      /*diffuse=*/glm::vec3(5.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-4.0f, 0.5f, -3.0f),
      /*diffuse=*/glm::vec3(10.0f, 0.0f, 0.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(3.0f, 0.5f, 1.0f),
      /*diffuse=*/glm::vec3(0.0f, 0.0f, 15.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-0.8f, 2.4f, -1.0f),
      /*diffuse=*/glm::vec3(0.0f, 5.0f, 0.0f)));

  qrk::Attenuation attenuation = {
      .constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
  for (auto& light : lights) {
    light->setSpecular(glm::vec3(0.0f));
    light->setAttenuation(attenuation);
    lightRegistry->addLight(light);
  }

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.0f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 1.032f);

  // Create the scene.
  std::vector<qrk::Mesh*> meshes;
  qrk::CubeMesh floor("examples/assets/wood.png");
  floor.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(25.0f, 1.0f, 25.0f)));
  meshes.push_back(&floor);

  qrk::CubeMesh cube1("examples/assets/container2.png");
  cube1.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 1.5f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube1);

  qrk::CubeMesh cube2("examples/assets/container2.png");
  cube2.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 1.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube2);

  qrk::CubeMesh cube3("examples/assets/container2.png");
  cube3.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-1.0f, -1.0f, 2.0f)),
                  glm::radians(60.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.0f)));
  meshes.push_back(&cube3);

  qrk::CubeMesh cube4("examples/assets/container2.png");
  cube4.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 2.7f, 4.0f)),
                  glm::radians(23.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.5f)));
  meshes.push_back(&cube4);

  qrk::CubeMesh cube5("examples/assets/container2.png");
  cube5.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-2.0f, 1.0f, -3.0f)),
                  glm::radians(124.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.0f)));
  meshes.push_back(&cube5);

  qrk::CubeMesh cube6("examples/assets/container2.png");
  cube6.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-3.0f, 0.0f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube6);

  // Framebuffer.
  qrk::Framebuffer mainFb(win.getSize());
  auto colorAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  auto bloomAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  mainFb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  qrk::ScreenQuadMesh screenQuad;
  qrk::ScreenQuadMesh bloomQuad(bloomAttachment.asTexture());

  // Create ping-pong buffers for blurring.
  qrk::Framebuffer blurFb1(win.getSize());
  qrk::Framebuffer blurFb2(win.getSize());
  auto blurAttachment1 =
      blurFb1.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  auto blurAttachment2 =
      blurFb2.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);

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

    // Draw meshes and lights
    for (auto mesh : meshes) {
      mesh->draw(mainShader);
    }

    lampShader.updateUniforms();
    for (auto& light : lights) {
      qrk::CubeMesh lightCube;
      lightCube.setModelTransform(glm::scale(
          glm::translate(glm::mat4(), light->getPosition()), glm::vec3(1.0f)));
      lampShader.setVec3("lightColor", light->getDiffuse());
      lightCube.draw(lampShader);
    }

    mainFb.deactivate();

    win.setViewport();
    screenShader.updateUniforms();

    if (drawOption == 1) {
      screenQuad.setTexture(bloomAttachment);
      screenQuad.draw(screenShader);
      return;
    }

    // Ping-pong between horizontal and vertical gaussian blurs.
    // TODO: Extract ping-pong to a helper?
    qrk::Framebuffer* pingPongFbs[2] = {&blurFb1, &blurFb2};
    qrk::Attachment* pingPongAttachments[2] = {&blurAttachment1,
                                               &blurAttachment2};
    bool horizontal = false;
    qrk::Framebuffer* pingPongFb = pingPongFbs[horizontal];
    qrk::Attachment* pingPongAttachment = &bloomAttachment;

    // 5 horizontal, 5 vertical.
    constexpr int totalIterations = 10;
    for (int i = 0; i < totalIterations; i++) {
      pingPongFb->activate();

      blurShader.setBool("horizontal", horizontal);
      screenQuad.setTexture(*pingPongAttachment);
      screenQuad.draw(blurShader);

      horizontal = !horizontal;
      // For a given 'horizontal' setting, we render _from_ the other mesh
      // (non-horizontal) to the corresponding framebuffer.
      pingPongFb = pingPongFbs[horizontal];
      pingPongAttachment = pingPongAttachments[!horizontal];

      pingPongFb->deactivate();
    }

    qrk::Attachment* blurredAttachment = pingPongAttachment;

    if (drawOption == 2) {
      screenQuad.setTexture(*blurredAttachment);
      screenQuad.draw(screenShader);
      return;
    }

    // drawOption == 0
    qrk::Texture bloomTexture = blurredAttachment->asTexture();
    bloomTexture.bindToUnit(1);
    bloomScreenShader.setBool("useBloom", useBloom);
    bloomScreenShader.setInt("bloomTexture", 1);
    screenQuad.setTexture(colorAttachment);
    screenQuad.draw(bloomScreenShader);
  });

  return 0;
}
