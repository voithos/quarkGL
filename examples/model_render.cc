// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/flags/usage.h"

ABSL_FLAG(std::string, model, "", "Path to a model file");

const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0); }
)SHADER";

const char* normalShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0, 1.0, 0.0, 1.0); }
)SHADER";

/** Loads a model based on command line flag, or a default. */
std::unique_ptr<qrk::Model> loadModel() {
  std::string modelPath = absl::GetFlag(FLAGS_model);

  if (!modelPath.empty()) {
    return std::make_unique<qrk::Model>(modelPath.c_str());
  }

  // Default to the nanosuit.
  auto nanosuit =
      std::make_unique<qrk::Model>("examples/assets/nanosuit/nanosuit.obj");

  // Translate the model down so it's in the center and scale it down, since
  // it's too big.
  nanosuit->setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.75f, 0.0f)),
                 glm::vec3(0.2f)));
  return nanosuit;
}

int main(int argc, char** argv) {
  absl::SetProgramUsageMessage(
      "quarkGL model viewer. Usage:\n  model_render --model path/to/model.obj");
  absl::ParseCommandLine(argc, argv);

  qrk::Window win(800, 600, "Model render", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/phong.frag"));
  mainShader.addUniformSource(camera);

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::Shader normalShader(
      qrk::ShaderPath("examples/shaders/model.vert"),
      qrk::ShaderInline(normalShaderSource),
      qrk::ShaderPath("examples/shaders/model_normals.geom"));
  normalShader.addUniformSource(camera);

  qrk::SkyboxShader skyboxShader;
  skyboxShader.addUniformSource(camera);

  qrk::SkyboxMesh skybox({
      "examples/assets/skybox/right.jpg",
      "examples/assets/skybox/left.jpg",
      "examples/assets/skybox/top.jpg",
      "examples/assets/skybox/bottom.jpg",
      "examples/assets/skybox/front.jpg",
      "examples/assets/skybox/back.jpg",
  });

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  registry->setViewSource(camera);
  mainShader.addUniformSource(registry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(-0.2f, -1.0f, -0.3f));
  directionalLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(directionalLight);

  auto pointLight =
      std::make_shared<qrk::PointLight>(glm::vec3(1.2f, 1.0f, 2.0f));
  pointLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(pointLight);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);

  // Create a mesh for the light.
  // TODO: Make this into a sphere.
  qrk::CubeMesh lightCube;
  lightCube.setModelTransform(glm::scale(
      glm::translate(glm::mat4(), pointLight->getPosition()), glm::vec3(0.2f)));

  qrk::CubeMesh boxCube("examples/assets/container.jpg");
  boxCube.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.5f, 0.5f)),
                 glm::vec3(0.5f)));

  // Load model.
  std::unique_ptr<qrk::Model> model = loadModel();

  bool drawNormals = false;
  win.addKeyPressHandler(GLFW_KEY_1,
                         [&](int mods) { drawNormals = !drawNormals; });

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // Draw main models.
    // TODO: Set up environment mapping with the skybox.
    mainShader.updateUniforms();
    boxCube.draw(mainShader);
    model->draw(mainShader);

    if (drawNormals) {
      // Draw the normals.
      normalShader.updateUniforms();
      boxCube.draw(normalShader);
      model->draw(normalShader);
    }

    // Draw light source.
    lampShader.updateUniforms();
    lightCube.draw(lampShader);

    // Draw skybox.
    skyboxShader.updateUniforms();
    skybox.draw(skyboxShader);
  });

  return 0;
}
