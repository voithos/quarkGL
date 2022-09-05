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

  qrk::Window win(width, height, "Shadow map", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.5f, 5.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(
      qrk::ShaderPath("examples/shaders/shadowed_model.vert"),
      qrk::ShaderPath("examples/shaders/shadowed_phong.frag"));
  mainShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  registry->setViewSource(camera);
  mainShader.addUniformSource(registry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(2.0f, -4.0f, 1.0f));
  directionalLight->setSpecular(glm::vec3(0.3f, 0.3f, 0.3f));
  registry->addLight(directionalLight);

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  // Setup shadow mapping.
  constexpr int SHADOW_MAP_SIZE = 1024;
  qrk::Framebuffer shadowFb(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
  qrk::Texture shadowMap =
      shadowFb
          // TODO: Extract this into a reusable pattern.
          .attachTexture(qrk::BufferType::DEPTH,
                         {
                             .filtering = qrk::TextureFiltering::NEAREST,
                             .wrapMode = qrk::TextureWrapMode::CLAMP_TO_BORDER,
                             .borderColor = glm::vec4(1.0f),
                         })
          .asTexture();
  qrk::ShadowMapShader shadowShader;
  auto shadowCamera = std::make_shared<qrk::ShadowCamera>(directionalLight);
  shadowShader.addUniformSource(shadowCamera);
  mainShader.addUniformSource(shadowCamera);

  // Debug drawing shader.
  qrk::ScreenQuadMesh screenQuad(shadowMap);
  qrk::ScreenQuadShader screenShader;

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

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    shadowFb.activate();
    shadowFb.clear();
    shadowShader.updateUniforms();
    plane.draw(shadowShader);
    box1.draw(shadowShader);
    box2.draw(shadowShader);
    box3.draw(shadowShader);
    shadowFb.deactivate();

    win.setViewport();
    if (drawShadowMap) {
      screenQuad.draw(screenShader);
    } else {
      mainShader.updateUniforms();
      // TODO: Make this more generic.
      shadowMap.bindToUnit(10);
      mainShader.setInt("shadowMap", 10);
      plane.draw(mainShader);
      box1.draw(mainShader);
      box2.draw(mainShader);
      box3.draw(mainShader);
    }
  });

  return 0;
}
