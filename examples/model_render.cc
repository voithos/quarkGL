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

// TODO: Move this to a built in shader.
const char* skyboxVertShaderSource = R"SHADER(
#version 330 core
layout(location = 0) in vec3 vertexPos;

out vec3 skyboxCoords;

uniform mat4 view;
uniform mat4 projection;

void main() {
  // No model transform needed for a skybox.
  vec4 pos = projection * view * vec4(vertexPos, 1.0);
  // The skybox is meant to be drawn last, so to take advantage of early depth testing, we set the vertex's z component to w so that after the perspective division by w the resulting normalized device coordinate will equal 1.0, which is the maximum depth value. This allows the skybox to be rendered behind everything else.
  gl_Position = pos.xyww;
  // The sample coordinates are equivalent to the interpolated vertex positions.
  skyboxCoords = vertexPos;
}  
)SHADER";

const char* skyboxFragShaderSource = R"SHADER(
#version 330 core
out vec4 fragColor;

in vec3 skyboxCoords;

uniform samplerCube skybox;

void main() {    
  fragColor = texture(skybox, skyboxCoords);
}
)SHADER";

int main() {
  qrk::Window win(800, 600, "Model render", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::TOGGLE_MOUSE_CAPTURE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/model.vert"),
                         qrk::ShaderPath("examples/phong.frag"));
  mainShader.addUniformSource(camera);

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::Shader normalShader(qrk::ShaderPath("examples/model.vert"),
                           qrk::ShaderInline(normalShaderSource),
                           qrk::ShaderPath("examples/model_normals.geom"));
  normalShader.addUniformSource(camera);

  // Needs to be separate due to some stupid C++ compiler nonsense.
  auto skyboxFrag = qrk::ShaderInline(skyboxFragShaderSource);
  qrk::Shader skyboxShader(qrk::ShaderInline(skyboxVertShaderSource),
                           skyboxFrag);
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

  qrk::Shader lampShader(qrk::ShaderPath("examples/model.vert"),
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
  qrk::Model nanosuit("examples/assets/nanosuit/nanosuit.obj");

  // Translate the model down so it's in the center and scale it down, since
  // it's too big.
  nanosuit.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.75f, 0.0f)),
                 glm::vec3(0.2f)));

  bool drawNormals = false;
  win.addKeyPressHandler(GLFW_KEY_1,
                         [&](int mods) { drawNormals = !drawNormals; });

  win.enableCulling();
  win.loop([&](float deltaTime) {
    // Draw main models.
    // TODO: Set up environment mapping with the skybox.
    mainShader.updateUniforms();
    boxCube.draw(mainShader);
    nanosuit.draw(mainShader);

    if (drawNormals) {
      // Draw the normals.
      normalShader.updateUniforms();
      boxCube.draw(normalShader);
      nanosuit.draw(normalShader);
    }

    // Draw light source.
    lampShader.updateUniforms();
    lightCube.draw(lampShader);

    // Draw skybox. We disable depth writing so that it always appears behind.
    glDepthFunc(GL_LEQUAL);
    skyboxShader.updateUniforms();
    skyboxShader.setMat4("view",
                         glm::mat4(glm::mat3(camera->getViewTransform())));
    skybox.draw(skyboxShader);
    glDepthFunc(GL_LESS);
  });

  return 0;
}
