// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/exceptions.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/window.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  constexpr int width = 800, height = 600;

  qrk::Window win(width, height, "Shadow map", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::CLOSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/model.vert"),
                         qrk::ShaderPath("examples/phong.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  registry->setViewSource(camera);
  mainShader.addUniformSource(registry);

  // Need a fake "position" for the directional light in order to calculate
  // shadow maps.
  glm::vec3 directionalLightPos(-2.0f, 4.0f, -1.0f);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(-directionalLightPos);
  directionalLight->setSpecular(glm::vec3(0.3f, 0.3f, 0.3f));
  registry->addLight(directionalLight);

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::CubeMesh box1("examples/assets/wood.png");
  box1.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(0.0f, 1.5f, 0.0f)),
                 glm::vec3(0.5f)));
  qrk::CubeMesh box2("examples/assets/wood.png");
  box2.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 1.0f)),
                 glm::vec3(0.5f)));
  qrk::CubeMesh box3("examples/assets/wood.png");
  box3.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(1.0f, 0.0f, 1.0f)),
                  glm::radians(60.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(0.25f)));

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    mainShader.updateUniforms();
    box1.draw(mainShader);
    box2.draw(mainShader);
    box3.draw(mainShader);
  });

  return 0;
}
