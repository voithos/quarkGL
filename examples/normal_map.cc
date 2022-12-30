// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// TODO: Pull into a simple color shader.
const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

void main() { fragColor = vec4(1.0); }
)SHADER";

int main() {
  constexpr int width = 1280, height = 960;

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

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/normal_map.vert"),
                         qrk::ShaderPath("examples/shaders/normal_map.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  auto pointLight = std::make_shared<qrk::PointLight>(glm::vec3(0.5, 1.0, 0.7));
  pointLight->setDiffuse(glm::vec3(1.0f));
  pointLight->setSpecular(glm::vec3(0.2f));
  lightRegistry->addLight(pointLight);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);

  // Create a mesh for the light.
  // TODO: Make this into a sphere.
  qrk::CubeMesh lightCube;
  lightCube.setModelTransform(glm::scale(
      glm::translate(glm::mat4(), pointLight->getPosition()), glm::vec3(0.2f)));

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.1f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  auto textureRegistry = std::make_shared<qrk::TextureRegistry>();
  mainShader.addUniformSource(textureRegistry);

  // Setup objects.
  qrk::PlaneMesh plane("examples/assets/brickwall.jpg");

  qrk::Texture normalMap = qrk::Texture::load(
      "examples/assets/brickwall_normal.jpg", /*isSRGB=*/false);

  bool useVertexNormals = false;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) {
    useVertexNormals = !useVertexNormals;
    printf("useVertexNormals = %d\n", useVertexNormals);
  });
  bool renderNormals = false;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) {
    renderNormals = !renderNormals;
    printf("renderNormals = %d\n", renderNormals);
  });

  printf("Controls:\n");
  printf("- WASD: movement\n");
  printf("- Mouse: camera\n");
  printf("- 1: switch between normal map and vertex normals\n");
  printf("- 2: switch between lighting and rendering normals\n");

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    plane.setModelTransform(glm::scale(
        glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, 0.0f)),
                    glm::radians(qrk::time() * -20.0f),
                    glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))),
        glm::vec3(3.0f)));

    // Draw the plane.
    mainShader.updateUniforms();
    unsigned int textureUnit = textureRegistry->getNextTextureUnit();
    normalMap.bindToUnit(textureUnit);
    mainShader.setInt("normalMap", textureUnit);
    mainShader.setBool("useVertexNormals", useVertexNormals);
    mainShader.setBool("renderNormals", renderNormals);
    plane.draw(mainShader, textureRegistry.get());

    // Draw light source.
    lampShader.updateUniforms();
    lightCube.draw(lampShader);
  });

  return 0;
}
