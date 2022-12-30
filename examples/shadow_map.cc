// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main() {
  constexpr int width = 1280, height = 960;

  qrk::Window win(width, height, "Shadow map", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera = std::make_shared<qrk::Camera>(
      /* position */ glm::vec3(-0.5f, 1.5f, 6.0f));
  camera->lookAt(glm::vec3(0.5f, 0.0f, 0.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/shadow_map.vert"),
                         qrk::ShaderPath("examples/shaders/shadow_map.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(2.0f, -4.0f, 1.0f));
  directionalLight->setDiffuse(glm::vec3(0.9f));
  directionalLight->setSpecular(glm::vec3(0.3f));
  lightRegistry->addLight(directionalLight);

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.1f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  // Setup shadow mapping.
  constexpr int SHADOW_MAP_SIZE = 1024;
  auto shadowMap =
      std::make_shared<qrk::ShadowMap>(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  auto textureRegistry = std::make_shared<qrk::TextureRegistry>();
  textureRegistry->addTextureSource(shadowMap);
  mainShader.addUniformSource(textureRegistry);

  qrk::ShadowMapShader shadowShader;
  auto shadowCamera = std::make_shared<qrk::ShadowCamera>(directionalLight);
  shadowShader.addUniformSource(shadowCamera);
  mainShader.addUniformSource(shadowCamera);

  // Debug drawing shader.
  qrk::ScreenQuadMesh screenQuad(shadowMap->getDepthTexture());
  qrk::ScreenShader screenShader;

  // Setup objects.
  // TODO: Because we don't adjust UVs, this ends up looking goofy.
  qrk::PlaneMesh plane("examples/assets/wood.png");
  plane.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(0.0f, -0.5f, 0.0f)),
                 glm::vec3(125.0f)));

  qrk::CubeMesh box1("examples/assets/wood.png");
  box1.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(0.0f, 1.5f, 0.0f)),
                 glm::vec3(1.0f)));
  qrk::CubeMesh box2("examples/assets/wood.png");
  box2.setModelTransform(
      glm::scale(glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 1.0f)),
                 glm::vec3(1.0f)));
  qrk::CubeMesh box3("examples/assets/wood.png");
  box3.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, 2.0f)),
                  glm::radians(60.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(0.5f)));

  bool drawShadowMap = false;
  win.addKeyPressHandler(GLFW_KEY_1,
                         [&](int mods) { drawShadowMap = !drawShadowMap; });

  printf("Controls:\n");
  printf("- WASD: movement\n");
  printf("- Mouse: camera\n");
  printf("- 1: draw the shadow map\n");

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    shadowMap->activate();
    shadowMap->clear();
    shadowShader.updateUniforms();
    plane.draw(shadowShader);
    box1.draw(shadowShader);
    box2.draw(shadowShader);
    box3.draw(shadowShader);
    shadowMap->deactivate();

    win.setViewport();
    if (drawShadowMap) {
      screenQuad.draw(screenShader);
      return;
    }

    mainShader.updateUniforms();
    plane.draw(mainShader, textureRegistry.get());
    box1.draw(mainShader, textureRegistry.get());
    box2.draw(mainShader, textureRegistry.get());
    box3.draw(mainShader, textureRegistry.get());
  });

  return 0;
}
