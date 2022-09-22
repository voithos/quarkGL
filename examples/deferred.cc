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

  qrk::Window win(width, height, "Deferred rendering", /* fullscreen */ false,
                  /* samples */ 0);
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

  qrk::Shader geometryPassShader(
      qrk::ShaderPath("examples/shaders/deferred.vert"),
      qrk::ShaderPath("examples/shaders/deferred.frag"));
  geometryPassShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  // We explicitly _don't_ add the lightregistry to the geometry pass.
  // geometryPassShader.addUniformSource(lightRegistry);

  // mainShader.setFloat("material.shininess", 32.0f);
  // mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  // mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  // mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

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
            glm::rotate(glm::translate(glm::mat4(), pos), glm::radians(90.0f),
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
  });

  return 0;
}
