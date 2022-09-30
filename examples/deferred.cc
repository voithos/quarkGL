// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>
#include <glm/ext.hpp>

const char* lampShaderSource = R"SHADER(
#version 460 core
out vec4 fragColor;

uniform vec3 lightColor;

void main() { fragColor = vec4(lightColor, 1.0); }
)SHADER";

int main() {
  constexpr int width = 800, height = 600;

  qrk::Window win(width, height, "Deferred rendering", /* fullscreen */ false,
                  /* samples */ 0);
  // Need to use a zero clear color, or else the G-Buffer won't work properly.
  win.setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 5.0f));
  camera->setSpeed(10.0f);
  camera->lookAt(glm::vec3(0.0f, 0.0f, -1.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::DeferredGeometryPassShader geometryPassShader;
  geometryPassShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  // We explicitly _don't_ add the lightregistry to the geometry pass.
  // geometryPassShader.addUniformSource(lightRegistry);

  std::mt19937 gen(42);
  // Create random lights.
  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  constexpr int NUM_LIGHTS = 32;
  for (int i = 0; i < NUM_LIGHTS; i++) {
    auto light = std::make_shared<qrk::PointLight>(
        glm::vec3(((gen() % 100) / 100.0f) * 6.0f - 3.0f,
                  ((gen() % 100) / 100.0f) * 6.0f - 4.0f,
                  ((gen() % 100) / 100.0f) * 6.0f - 3.0f));

    light->setAmbient(glm::vec3(0.0f));
    glm::vec3 lightColor(
        ((gen() % 100) / 200.0f) + 0.5f,  // between 0.5 and 1.0
        ((gen() % 100) / 200.0f) + 0.5f,  // between 0.5 and 1.0
        ((gen() % 100) / 200.0f) + 0.5f   // between 0.5 and 1.0
    );
    light->setDiffuse(lightColor);
    light->setSpecular(lightColor);
    qrk::Attenuation attenuation = {
        .constant = 1.0f, .linear = 0.7f, .quadratic = 1.8f};
    light->setAttenuation(attenuation);

    lightRegistry->addLight(light);
    lights.push_back(light);
  }

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderInline(lampShaderSource));
  lampShader.addUniformSource(camera);
  qrk::CubeMesh lightCube;

  // Create the scene.
  std::vector<qrk::Renderable*> meshes;
  auto helmet = std::make_unique<qrk::Model>(
      "examples/assets/DamagedHelmet/DamagedHelmet.gltf");
  meshes.push_back(helmet.get());

  // clang-format off
  std::vector<glm::vec3> meshPositions = {
    glm::vec3(-3.0f,  0.0f, -3.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f),
    glm::vec3( 3.0f,  0.0f, -3.0f),
    glm::vec3(-3.0f,  0.0f,  0.0f),
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 3.0f,  0.0f,  0.0f),
    glm::vec3(-3.0f,  0.0f,  3.0f),
    glm::vec3( 0.0f,  0.0f,  3.0f),
    glm::vec3( 3.0f,  0.0f,  3.0f)
  };
  // clang-format on

  // Build the G-Buffer.
  // TODO: Standardize this pattern, and build a TextureSource so that it can
  // be easily used in a TextureRegistry.
  qrk::Framebuffer gBuffer(win.getSize());
  gBuffer.setClearColor(glm::vec4(0.0f));
  gBuffer.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  // Position and normal are stored as "HDR" colors for higher precision.
  // Alpha channels unused.
  auto positionBuffer = gBuffer.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  auto normalBuffer = gBuffer.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  // RGB used for albedo, A used for specularity.
  auto albedoSpecularBuffer =
      gBuffer.attachTexture(qrk::BufferType::COLOR_ALPHA);
  auto emissionBuffer = gBuffer.attachTexture(qrk::BufferType::COLOR_ALPHA);

  qrk::ScreenQuadMesh screenQuad;
  qrk::ScreenQuadShader gBufferVisShader(
      qrk::ShaderPath("examples/shaders/gbuffer.frag"));

  int gBufferVis = 0;
  constexpr int NUM_GBUFFER_VIS = 6;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) {
    gBufferVis = (gBufferVis + 1) % NUM_GBUFFER_VIS;
    switch (gBufferVis) {
      case 0:
        std::cout << "Switching to normal rendering" << std::endl;
        break;
      case 1:
        std::cout << "Drawing G-Buffer positions" << std::endl;
        break;
      case 2:
        std::cout << "Drawing G-Buffer normals" << std::endl;
        break;
      case 3:
        std::cout << "Drawing G-Buffer albedo" << std::endl;
        break;
      case 4:
        std::cout << "Drawing G-Buffer specularity" << std::endl;
        break;
      case 5:
        std::cout << "Drawing G-Buffer emission" << std::endl;
        break;
    };
  });

  // Set up the lighting pass.
  qrk::ScreenQuadShader lightingPassShader(
      qrk::ShaderPath("examples/shaders/deferred_lighting.frag"));
  lightingPassShader.addUniformSource(camera);
  lightingPassShader.addUniformSource(lightRegistry);
  lightingPassShader.setFloat("shininess", 16.0f);
  lightingPassShader.setFloat("emissionAttenuation.constant", 1.0f);
  lightingPassShader.setFloat("emissionAttenuation.linear", 0.09f);
  lightingPassShader.setFloat("emissionAttenuation.quadratic", 0.032f);

  // win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // Step 1: geometry pass. Build the G-Buffer.
    gBuffer.activate();
    gBuffer.clear();

    geometryPassShader.updateUniforms();

    // Draw meshes.
    for (auto mesh : meshes) {
      for (auto pos : meshPositions) {
        mesh->setModelTransform(glm::scale(
            glm::rotate(glm::translate(glm::mat4(), pos), glm::radians(0.0f),
                        glm::vec3(1.0f, 0.0f, 0.0f)),
            glm::vec3(1.0f)));
        mesh->draw(geometryPassShader);
      }
    }

    gBuffer.deactivate();

    win.setViewport();

    if (gBufferVis > 0) {
      switch (gBufferVis) {
        case 1:
          screenQuad.setTexture(positionBuffer);
          break;
        case 2:
          screenQuad.setTexture(normalBuffer);
          break;
        case 3:
        case 4:
          screenQuad.setTexture(albedoSpecularBuffer);
          break;
        case 5:
          screenQuad.setTexture(emissionBuffer);
          break;
      };
      gBufferVisShader.setInt("gBufferVis", gBufferVis);
      screenQuad.draw(gBufferVisShader);
      return;
    }
    // Continue.

    // Step 2: lighting pass.

    lightingPassShader.updateUniforms();
    // Bind textures.
    screenQuad.unsetTexture();
    positionBuffer.asTexture().bindToUnit(0);
    normalBuffer.asTexture().bindToUnit(1);
    albedoSpecularBuffer.asTexture().bindToUnit(2);
    emissionBuffer.asTexture().bindToUnit(3);
    // Bind sampler uniforms.
    lightingPassShader.setInt("gPosition", 0);
    lightingPassShader.setInt("gNormal", 1);
    lightingPassShader.setInt("gAlbedoSpecular", 2);
    lightingPassShader.setInt("gEmission", 3);

    screenQuad.draw(lightingPassShader);

    // Step 3: forward render anything else on top.

    // Before we do so, we have to blit the depth buffer.
    gBuffer.blitToDefault(GL_DEPTH_BUFFER_BIT);

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
