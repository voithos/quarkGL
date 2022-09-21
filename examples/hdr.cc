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

  qrk::Window win(width, height, "HDR", /* fullscreen */ false,
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
                         qrk::ShaderPath("examples/shaders/phong.frag"));
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

  // Framebuffer.
  qrk::Framebuffer fb(win.getSize());
  auto colorAttachment = fb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  fb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  qrk::ScreenQuadMesh screenQuad(colorAttachment.asTexture());
  qrk::ScreenQuadShader screenShader(
      qrk::ShaderPath("examples/shaders/hdr.frag"));

  // Setup the tunnel.
  qrk::CubeMesh tunnel("examples/assets/wood.png");
  tunnel.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 25.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(5.0f, 5.0f, 55.0f)));

  bool useHdr = true;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) { useHdr = !useHdr; });
  int toneMapTechnique = 0;
  constexpr int NUM_TONEMAP_TECHNIQUES = 3;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) {
    toneMapTechnique = (toneMapTechnique + 1) % NUM_TONEMAP_TECHNIQUES;
  });

  win.loop([&](float deltaTime) {
    // Draw the scene to the HDR framebuffer.
    fb.activate();
    fb.clear();

    mainShader.updateUniforms();
    mainShader.setBool("inverseNormals", true);
    mainShader.setBool("skipGamma", true);
    tunnel.draw(mainShader);

    fb.deactivate();

    win.setViewport();
    screenShader.updateUniforms();
    screenShader.setBool("useHdr", useHdr);
    screenShader.setInt("toneMapTechnique", toneMapTechnique);
    screenQuad.draw(screenShader);
  });

  return 0;
}