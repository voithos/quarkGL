// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/camera.h>
#include <qrk/light.h>
#include <qrk/mesh_primitives.h>
#include <qrk/model.h>
#include <qrk/shader.h>
#include <qrk/vertex_array.h>
#include <qrk/window.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const char* lampShaderSource = R"SHADER(
#version 330 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0); }
)SHADER";

const char* normalShaderSource = R"SHADER(
#version 330 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0, 1.0, 0.0, 1.0); }
)SHADER";

int main() {
  qrk::Window win(800, 600, "Model render", /* fullscreen */ false,
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

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::Shader normalShader(qrk::ShaderPath("examples/model.vert"),
                           qrk::ShaderInline(normalShaderSource),
                           qrk::ShaderPath("examples/model_normals.geom"));

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  mainShader.addUniformSource(registry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(-0.2f, -1.0f, -0.3f));
  directionalLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(directionalLight);

  auto pointLight =
      std::make_shared<qrk::PointLight>(glm::vec3(1.2f, 1.0f, 2.0f));
  pointLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(pointLight);

  qrk::Shader lampShader(qrk::ShaderPath("examples/model.vert"),
                         qrk::ShaderInline(lampShaderSource));

  // Create a mesh for the light.
  // TODO: Make this into a sphere.
  qrk::CubeMesh lightCube;

  // Load model.
  qrk::Model nanosuit("examples/assets/nanosuit/nanosuit.obj");

  bool drawNormals = false;
  win.addKeyPressHandler(GLFW_KEY_1,
                         [&](int mods) { drawNormals = !drawNormals; });

  win.enableCulling();
  win.loop([&](float deltaTime) {
    glm::mat4 view = camera->getViewTransform();
    glm::mat4 projection = camera->getPerspectiveTransform();

    // Setup shader and lights.
    mainShader.activate();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);

    registry->applyViewTransform(view);
    mainShader.updateUniforms();

    // Draw main model.
    // Translate the model down so it's in the center.
    glm::mat4 model =
        glm::translate(glm::mat4(), glm::vec3(0.0f, -1.75f, 0.0f));
    // Scale it down, since it's too big.
    model = glm::scale(model, glm::vec3(0.2f));
    mainShader.setMat4("model", model);

    nanosuit.draw(mainShader);

    if (drawNormals) {
      // Draw the normals.
      normalShader.activate();
      normalShader.setMat4("model", model);
      normalShader.setMat4("view", view);
      normalShader.setMat4("projection", projection);
      nanosuit.draw(normalShader);
    }

    // Draw light source.
    glm::mat4 lightModel =
        glm::translate(glm::mat4(), pointLight->getPosition());
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    lampShader.activate();
    lampShader.setMat4("model", lightModel);
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);
    lightCube.draw(lampShader);
  });

  return 0;
}
