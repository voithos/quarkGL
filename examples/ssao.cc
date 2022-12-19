// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

uniform vec3 lightColor;

void main() { fragColor = vec4(lightColor, 1.0); }
)SHADER";

int main() {
  constexpr int width = 800, height = 600;

  qrk::Window win(width, height, "SSAO", /* fullscreen */ false,
                  /* samples */ 0);
  win.setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  // Create camera.
  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 5.0f));
  camera->setSpeed(10.0f);
  camera->lookAt(glm::vec3(0.0f, 0.0f, -1.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  // We explicitly _don't_ add the lightregistry to the geometry pass.
  // geometryPassShader.addUniformSource(lightRegistry);

  // Create lights.
  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  auto light = std::make_shared<qrk::PointLight>(glm::vec3(0.0f, 5.0f, 0.0f));
  light->setDiffuse(glm::vec3(0.3f));
  light->setSpecular(light->getDiffuse());
  qrk::Attenuation attenuation = {
      .constant = 1.0f, .linear = 0.09f, .quadratic = 0.032f};
  light->setAttenuation(attenuation);
  lightRegistry->addLight(light);
  lights.push_back(light);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(-0.2f, -1.0f, -0.3f));
  directionalLight->setSpecular(glm::vec3(0.1f, 0.1f, 0.1f));
  lightRegistry->addLight(directionalLight);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);
  qrk::CubeMesh lightCube;

  // Create the scene.
  qrk::Model backpack("examples/assets/SurvivalGuitarBackpack/scene.gltf");
  qrk::TextureMap brickwall(qrk::Texture::load("examples/assets/brickwall.jpg"),
                            qrk::TextureMapType::DIFFUSE);
  qrk::TextureMap brickwallNormalMap(
      qrk::Texture::load("examples/assets/brickwall_normal.jpg",
                         /*isSRGB=*/false),
      qrk::TextureMapType::NORMAL);
  qrk::RoomMesh room({brickwall, brickwallNormalMap});

  // Build the G-Buffer and prepare deferred shading.
  qrk::DeferredGeometryPassShader geometryPassShader;
  geometryPassShader.addUniformSource(camera);

  auto gBuffer = std::make_shared<qrk::GBuffer>(win.getSize());

  // Build SSAO buffers.
  qrk::SsaoShader ssaoShader;
  ssaoShader.addUniformSource(camera);

  auto ssaoKernel = std::make_shared<qrk::SsaoKernel>();
  ssaoShader.addUniformSource(ssaoKernel);
  auto ssaoBuffer = std::make_shared<qrk::SsaoBuffer>(win.getSize());

  auto ssaoPassTextures = std::make_shared<qrk::TextureRegistry>();
  ssaoPassTextures->addTextureSource(gBuffer);
  ssaoPassTextures->addTextureSource(ssaoKernel);
  ssaoShader.addUniformSource(ssaoPassTextures);

  qrk::SsaoBlurShader ssaoBlurShader;
  auto ssaoBlurredBuffer = std::make_shared<qrk::SsaoBuffer>(win.getSize());

  auto lightingPassTextures = std::make_shared<qrk::TextureRegistry>();
  lightingPassTextures->addTextureSource(gBuffer);
  lightingPassTextures->addTextureSource(ssaoBlurredBuffer);

  qrk::ScreenQuadMesh screenQuad;

  // Set up the lighting pass.
  qrk::ScreenQuadShader lightingPassShader(
      qrk::ShaderPath("examples/shaders/deferred_lighting_ssao.frag"));
  lightingPassShader.addUniformSource(lightRegistry);
  lightingPassShader.addUniformSource(lightingPassTextures);
  lightingPassShader.setVec3("ambient", glm::vec3(0.5f));
  lightingPassShader.setFloat("shininess", 16.0f);
  lightingPassShader.setFloat("emissionAttenuation.constant", 1.0f);
  lightingPassShader.setFloat("emissionAttenuation.linear", 0.09f);
  lightingPassShader.setFloat("emissionAttenuation.quadratic", 0.032f);

  qrk::ScreenQuadShader screenShader;

  bool drawOcclusionMap = false;
  win.addKeyPressHandler(
      GLFW_KEY_1, [&](int mods) { drawOcclusionMap = !drawOcclusionMap; });
  bool useSsao = true;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) { useSsao = !useSsao; });

  // win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // Step 1: geometry pass. Build the G-Buffer.
    gBuffer->activate();
    gBuffer->clear();

    geometryPassShader.updateUniforms();

    // Draw meshes.
    backpack.setModelTransform(glm::scale(
        glm::rotate(glm::translate(glm::mat4(), glm::vec3(0, -2.2f, 0)),
                    glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::vec3(0.007f)));
    backpack.draw(geometryPassShader);
    room.setModelTransform(glm::scale(
        glm::rotate(glm::translate(glm::mat4(), glm::vec3(0, 5.0f, 0)),
                    glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::vec3(15.0f)));
    room.draw(geometryPassShader);

    gBuffer->deactivate();

    // Step 2: SSAO.
    ssaoBuffer->activate();
    ssaoBuffer->clear();

    ssaoShader.updateUniforms();

    screenQuad.unsetTexture();
    screenQuad.draw(ssaoShader, ssaoPassTextures.get());

    ssaoBuffer->deactivate();

    // Step 3: SSAO blur.
    ssaoBlurredBuffer->activate();
    ssaoBlurredBuffer->clear();

    ssaoBlurShader.configureWith(*ssaoKernel, *ssaoBuffer);
    screenQuad.unsetTexture();
    screenQuad.draw(ssaoBlurShader);

    ssaoBlurredBuffer->deactivate();

    if (drawOcclusionMap) {
      win.setViewport();
      screenQuad.setTexture(ssaoBlurredBuffer->getSsaoTexture());
      screenQuad.draw(screenShader);
      return;
    }

    // Step 4: lighting pass.
    win.setViewport();

    lightingPassShader.updateUniforms();
    lightingPassShader.setBool("useSsao", useSsao);
    // Bind textures.
    screenQuad.unsetTexture();
    screenQuad.draw(lightingPassShader, lightingPassTextures.get());

    // Step 5: forward render anything else on top.

    // Before we do so, we have to blit the depth buffer.
    gBuffer->blitToDefault(GL_DEPTH_BUFFER_BIT);

    // Draw the lights.
    lampShader.updateUniforms();
    for (auto& light : lights) {
      lightCube.setModelTransform(glm::scale(
          glm::translate(glm::mat4(), light->getPosition()), glm::vec3(0.2f)));
      lampShader.setVec3("lightColor", light->getDiffuse());
      lightCube.draw(lampShader);
    }
  });

  return 0;
}
