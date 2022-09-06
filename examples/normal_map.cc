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

  qrk::Window win(width, height, "Normal map", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(2.0f, 2.0f, 3.0f));
  camera->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/normal_model.vert"),
                         qrk::ShaderPath("examples/shaders/normal_phong.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  auto pointLight = std::make_shared<qrk::PointLight>(glm::vec3(0.5, 1.0, 0.3));
  pointLight->setDiffuse(glm::vec3(1.0f));
  pointLight->setSpecular(glm::vec3(0.2f));
  lightRegistry->addLight(pointLight);

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  auto textureRegistry = std::make_shared<qrk::TextureRegistry>();
  mainShader.addUniformSource(textureRegistry);

  // Setup objects.
  qrk::PlaneMesh plane("examples/assets/brickwall.jpg");
  plane.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
      glm::vec3(4.0f)));

  qrk::Texture normalMap =
      qrk::Texture::load("examples/assets/brickwall_normal.jpg");

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    mainShader.updateUniforms();
    unsigned int textureUnit = textureRegistry->getNextTextureUnit();
    normalMap.bindToUnit(textureUnit);
    mainShader.setInt("normalMap", textureUnit);
    plane.draw(mainShader, textureRegistry.get());
  });

  return 0;
}