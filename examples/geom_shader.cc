// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0); }
)SHADER";

int main() {
  qrk::Window win(1280, 960, "Geometry shader");
  win.setClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(3.0f, 0.0f, 3.0f));
  camera->lookAt(glm::vec3(0.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/phong.frag"),
                         qrk::ShaderPath("examples/shaders/explode.geom"));

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.1f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

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

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));

  // Create a mesh for the light.
  // TODO: Make this into a sphere.
  qrk::CubeMesh lightCube;

  // Load model.
  qrk::Model nanosuit("examples/assets/nanosuit/nanosuit.obj");

  printf("Controls:\n");
  printf("- WASD: movement\n");
  printf("- Mouse: camera\n");

  win.disableFaceCull();
  win.loop([&](float deltaTime) {
    glm::mat4 view = camera->getViewTransform();
    glm::mat4 projection = camera->getProjectionTransform();

    // Setup shader and lights.
    mainShader.activate();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);

    registry->applyViewTransform(view);
    mainShader.updateUniforms();

    // Draw main model.
    // Translate the model down so it's in the center.
    // Scale it down, since it's too big.
    nanosuit.setModelTransform(
        glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.75f, 0.0f)),
                   glm::vec3(0.2f)));

    nanosuit.draw(mainShader);

    // Draw light source.
    glm::mat4 lightModel =
        glm::translate(glm::mat4(1.0f), pointLight->getPosition());
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    lightCube.setModelTransform(lightModel);
    lampShader.activate();
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);
    lightCube.draw(lampShader);
  });

  return 0;
}
