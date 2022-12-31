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

uniform vec3 lightColor;

void main() { fragColor = vec4(lightColor, 1.0); }
)SHADER";

int main() {
  constexpr int width = 1280, height = 960;

  qrk::Window win(width, height, "PBR", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera = std::make_shared<qrk::Camera>(
      /* position */ glm::vec3(0.0f, 0.0f, 22.0f));
  camera->lookAt(glm::vec3(0.0f, 0.0f, -1.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  cameraControls->setSpeed(10.0f);
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader pbrShader(qrk::ShaderPath("examples/shaders/normal_map.vert"),
                        qrk::ShaderPath("examples/shaders/pbr.frag"));
  pbrShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  pbrShader.addUniformSource(lightRegistry);

  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-10.0f, 10.0f, 10.0f),
      /*diffuse=*/glm::vec3(300.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(10.0f, 10.0f, 10.0f),
      /*diffuse=*/glm::vec3(300.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-10.0f, -10.0f, 10.0f),
      /*diffuse=*/glm::vec3(300.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(10.0f, -10.0f, 10.0f),
      /*diffuse=*/glm::vec3(300.0f)));

  // Only use quadratic attenuation to match inverse square law.
  qrk::Attenuation attenuation = {
      .constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
  for (auto& light : lights) {
    light->setSpecular(light->getDiffuse());
    light->setAttenuation(attenuation);
    lightRegistry->addLight(light);
  }

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);
  qrk::CubeMesh lightCube;

  // TODO: Pull this out into a material class.
  pbrShader.setVec3("baseColor", glm::vec3(0.5f, 0.0f, 0.0f));
  pbrShader.setVec3("material.ambient", glm::vec3(0.03f));
  pbrShader.setFloat("material.shininess", 32.0f);
  pbrShader.setFloat("material.emissionAttenuation.constant", 0.0f);
  pbrShader.setFloat("material.emissionAttenuation.linear", 0.0f);
  pbrShader.setFloat("material.emissionAttenuation.quadratic", 1.0f);

  // Prepare the sphere.
  constexpr int NUM_SPHERE_ROWS = 7;
  constexpr int NUM_SPHERE_COLS = 7;
  constexpr float SPHERE_SPACING = 2.5;
  // Reuse one sphere for all.
  qrk::TextureMap rustedironBasecolor(
      qrk::Texture::load(
          "examples/assets/rusted_iron/rustediron2_basecolor.png"),
      qrk::TextureMapType::DIFFUSE);
  qrk::TextureMap rustedironNormalMap(
      qrk::Texture::load("examples/assets/rusted_iron/rustediron2_normal.png",
                         /*isSRGB=*/false),
      qrk::TextureMapType::NORMAL);
  qrk::TextureMap rustedironSpecularMap(
      qrk::Texture::load("examples/assets/rusted_iron/rustediron2_metallic.png",
                         /*isSRGB=*/false),
      qrk::TextureMapType::SPECULAR);
  qrk::SphereMesh sphere(
      {rustedironBasecolor, rustedironNormalMap, rustedironSpecularMap});

  bool usePBR = true;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) {
    usePBR = !usePBR;
    printf("usePBR = %d\n", usePBR);
  });
  bool useTextures = false;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) {
    useTextures = !useTextures;
    printf("useTextures = %d\n", useTextures);
  });

  printf("Controls:\n");
  printf("- WASD: movement\n");
  printf("- Mouse: camera\n");
  printf("- 1: switch between PBR and Phong\n");
  printf("- 2: switch between textures and single-color\n");
  printf(
      "Rendering %dx%d grid of spheres with varying roughness (X) and metallic "
      "(Y)\n",
      NUM_SPHERE_COLS, NUM_SPHERE_ROWS);

  win.loop([&](float deltaTime) {
    pbrShader.updateUniforms();

    pbrShader.setBool("usePBR", usePBR);
    pbrShader.setBool("useTextures", useTextures);

    // Draw spheres.
    for (int r = 0; r < NUM_SPHERE_ROWS; ++r) {
      pbrShader.setFloat("metallic", r / static_cast<float>(NUM_SPHERE_ROWS));

      for (int c = 0; c < NUM_SPHERE_COLS; ++c) {
        pbrShader.setFloat("roughness",
                           c / static_cast<float>(NUM_SPHERE_COLS));

        float xOffset = (c - (NUM_SPHERE_COLS / 2)) * SPHERE_SPACING;
        float yOffset = (r - (NUM_SPHERE_ROWS / 2)) * SPHERE_SPACING;

        sphere.setModelTransform(glm::scale(
            glm::rotate(
                glm::translate(glm::mat4(), glm::vec3(xOffset, yOffset, 0.0f)),
                glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
            glm::vec3(1.0f)));

        sphere.draw(pbrShader);
      }
    }

    // Draw lights.
    lampShader.updateUniforms();
    for (auto& light : lights) {
      lightCube.setModelTransform(glm::scale(
          glm::translate(glm::mat4(), light->getPosition()), glm::vec3(1.0f)));
      lampShader.setVec3("lightColor", light->getDiffuse());
      lightCube.draw(lampShader);
    }
  });

  return 0;
}
