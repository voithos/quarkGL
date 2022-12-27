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

  qrk::Window win(width, height, "Bloom", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 5.0f));
  camera->lookAt(glm::vec3(0.0f, 0.0f, 10.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  cameraControls->setSpeed(10.0f);
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/bloom.frag"));
  mainShader.addUniformSource(camera);

  qrk::Shader lampShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/bloom_lamp.frag"));
  lampShader.addUniformSource(camera);

  // Create light registry and add lights.
  auto lightRegistry = std::make_shared<qrk::LightRegistry>();
  lightRegistry->setViewSource(camera);
  mainShader.addUniformSource(lightRegistry);

  std::vector<std::shared_ptr<qrk::PointLight>> lights;
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(0.0f, 0.5f, 1.5f),
      /*diffuse=*/glm::vec3(5.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-4.0f, 0.5f, -3.0f),
      /*diffuse=*/glm::vec3(10.0f, 0.0f, 0.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(3.0f, 0.5f, 1.0f),
      /*diffuse=*/glm::vec3(0.0f, 0.0f, 15.0f)));
  lights.push_back(std::make_shared<qrk::PointLight>(
      /*position=*/glm::vec3(-0.8f, 2.4f, -1.0f),
      /*diffuse=*/glm::vec3(0.0f, 5.0f, 0.0f)));

  qrk::Attenuation attenuation = {
      .constant = 0.0f, .linear = 0.0f, .quadratic = 1.0f};
  for (auto& light : lights) {
    light->setSpecular(glm::vec3(0.0f));
    light->setAttenuation(attenuation);
    lightRegistry->addLight(light);
  }

  // TODO: Pull this out into a material class.
  mainShader.setVec3("material.ambient", glm::vec3(0.0f));
  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 1.032f);

  // Create the scene.
  std::vector<qrk::Mesh*> meshes;
  qrk::CubeMesh floor("examples/assets/wood.png");
  floor.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(25.0f, 1.0f, 25.0f)));
  meshes.push_back(&floor);

  qrk::CubeMesh cube1("examples/assets/container2.png");
  cube1.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 1.5f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube1);

  qrk::CubeMesh cube2("examples/assets/container2.png");
  cube2.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 1.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube2);

  qrk::CubeMesh cube3("examples/assets/container2.png");
  cube3.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-1.0f, -1.0f, 2.0f)),
                  glm::radians(60.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.0f)));
  meshes.push_back(&cube3);

  qrk::CubeMesh cube4("examples/assets/container2.png");
  cube4.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(0.0f, 2.7f, 4.0f)),
                  glm::radians(23.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.5f)));
  meshes.push_back(&cube4);

  qrk::CubeMesh cube5("examples/assets/container2.png");
  cube5.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-2.0f, 1.0f, -3.0f)),
                  glm::radians(124.0f),
                  glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f))),
      glm::vec3(2.0f)));
  meshes.push_back(&cube5);

  qrk::CubeMesh cube6("examples/assets/container2.png");
  cube6.setModelTransform(glm::scale(
      glm::rotate(glm::translate(glm::mat4(), glm::vec3(-3.0f, 0.0f, 0.0f)),
                  glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
      glm::vec3(1.0f)));
  meshes.push_back(&cube6);

  // Framebuffer.
  qrk::Framebuffer mainFb(win.getSize());
  auto colorAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  auto bloomAttachment = mainFb.attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  mainFb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  qrk::ScreenQuadMesh screenQuad;
  qrk::ScreenQuadMesh bloomQuad(bloomAttachment.asTexture());

  // Create ping-pong buffers for blurring.
  qrk::PingPongBuffer blurBuffer(win.getSize());
  qrk::GaussianBlurShader blurShader;

  // Create resampling buffers for resample based bloom.
  qrk::BloomBuffer bloomBuffer(win.getSize());
  int numResampleMips = bloomBuffer.getNumMips();
  std::cout << "Number of mip levels for resampling: " << numResampleMips
            << std::endl;
  qrk::BloomDownsampleShader bloomDownsampleShader;
  qrk::BloomUpsampleShader bloomUpsampleShader;

  qrk::ScreenShader screenShader(qrk::ShaderPath("examples/shaders/hdr.frag"));
  screenShader.setBool("useHdr", false);
  qrk::ScreenLodShader screenLodShader;
  qrk::ScreenShader bloomScreenShader(
      qrk::ShaderPath("examples/shaders/bloom_screen.frag"));

  bool useBloom = true;
  win.addKeyPressHandler(GLFW_KEY_1, [&](int mods) { useBloom = !useBloom; });
  bool useResampleBloom = true;
  win.addKeyPressHandler(GLFW_KEY_2, [&](int mods) {
    useResampleBloom = !useResampleBloom;
    std::cout << "useResampleBloom = " << useResampleBloom << std::endl;
  });

  int drawOptionResampleBloom = 0;
  const int NUM_DRAW_OPTIONS_RESAMPLE_BLOOM = numResampleMips + 1;
  win.addKeyPressHandler(GLFW_KEY_3, [&](int mods) {
    drawOptionResampleBloom =
        (drawOptionResampleBloom + 1) % NUM_DRAW_OPTIONS_RESAMPLE_BLOOM;
    std::cout << "drawOptionResampleBloom = " << drawOptionResampleBloom
              << std::endl;
  });

  int drawOptionBlurBloom = 0;
  constexpr int NUM_DRAW_OPTIONS_BLUR_BLOOM = 3;
  win.addKeyPressHandler(GLFW_KEY_4, [&](int mods) {
    drawOptionBlurBloom =
        (drawOptionBlurBloom + 1) % NUM_DRAW_OPTIONS_BLUR_BLOOM;
    std::cout << "drawOptionBlurBloom = " << drawOptionBlurBloom << std::endl;
  });

  // win.enableFaceCull();
  win.loop([&](float deltaTime) {
    // Draw the scene to the HDR framebuffer.
    mainFb.activate();
    mainFb.clear();

    mainShader.updateUniforms();

    // Draw meshes and lights
    for (auto mesh : meshes) {
      mesh->draw(mainShader);
    }

    lampShader.updateUniforms();
    for (auto& light : lights) {
      qrk::CubeMesh lightCube;
      lightCube.setModelTransform(glm::scale(
          glm::translate(glm::mat4(), light->getPosition()), glm::vec3(1.0f)));
      lampShader.setVec3("lightColor", light->getDiffuse());
      lightCube.draw(lampShader);
    }

    mainFb.deactivate();

    win.setViewport();
    screenShader.updateUniforms();

    if (useResampleBloom) {
      // Resampling bloom technique.
      screenQuad.unsetTexture();

      // Copy to mip level 0.
      bloomBuffer.activate(0);
      mainFb.blit(bloomBuffer, GL_COLOR_BUFFER_BIT);

      // Perform the downsampling across the mip chain.
      bloomDownsampleShader.configureWith(bloomBuffer);
      for (int destMip = 1; destMip < numResampleMips; ++destMip) {
        bloomBuffer.activate(destMip);
        int sourceMip = destMip - 1;
        bloomBuffer.setSourceMip(sourceMip);
        screenQuad.draw(bloomDownsampleShader);
      }

      // Perform the upsampling, starting with the second-to-last mip. We enable
      // additive blending to avoid having to render into a separate texture.
      bloomBuffer.enableAdditiveBlending();
      bloomUpsampleShader.configureWith(bloomBuffer);
      for (int destMip = numResampleMips - 2; destMip >= 0; --destMip) {
        bloomBuffer.activate(destMip);
        int sourceMip = destMip + 1;
        bloomBuffer.setSourceMip(sourceMip);
        screenQuad.draw(bloomUpsampleShader);
      }

      bloomBuffer.unsetSourceMip();
      bloomBuffer.disableAdditiveBlending();
      bloomBuffer.deactivate();
      win.setViewport();

      // Debug, look at the first 4 downsampled mips.
      if (drawOptionResampleBloom >= 1 &&
          drawOptionResampleBloom < numResampleMips + 1) {
        int mipTarget = drawOptionResampleBloom - 1;
        // screenLodShader.setMipLevel(drawOptionResampleBloom);
        bloomBuffer.setSourceMip(mipTarget);
        screenQuad.setTexture(bloomBuffer.getBloomMipChainTexture());
        screenQuad.draw(screenShader);
        bloomBuffer.unsetSourceMip();
        return;
      }

      // drawOptionResampleBloom == 0
      qrk::Texture bloomTexture = bloomBuffer.getBloomMipChainTexture();
      bloomTexture.bindToUnit(1);
      bloomScreenShader.setInt("bloomTexture", 1);
      bloomScreenShader.setBool("useBloom", useBloom);
      bloomScreenShader.setBool("interpolateBloom", true);
      bloomScreenShader.setFloat("bloomStrength", 0.04f);
      screenQuad.setTexture(colorAttachment);
      screenQuad.draw(bloomScreenShader);
    } else {
      // Gaussian blur based bloom effect.
      if (drawOptionBlurBloom == 1) {
        screenQuad.setTexture(bloomAttachment);
        screenQuad.draw(screenShader);
        return;
      }

      // Ping-pong between horizontal and vertical gaussian blurs.
      // 5 horizontal, 5 vertical.
      blurBuffer.multipassDraw(
          bloomAttachment.asTexture(), blurShader, /*passes=*/5,
          [&] { blurShader.setHorizontal(!blurShader.getHorizontal()); });

      qrk::Texture blurredTexture = blurBuffer.getOutput();

      if (drawOptionBlurBloom == 2) {
        screenQuad.setTexture(blurredTexture);
        screenQuad.draw(screenShader);
        return;
      }

      // drawOptionBlurBloom == 0
      qrk::Texture bloomTexture = blurredTexture;
      bloomTexture.bindToUnit(1);
      bloomScreenShader.setInt("bloomTexture", 1);
      bloomScreenShader.setBool("useBloom", useBloom);
      bloomScreenShader.setBool("interpolateBloom", false);
      screenQuad.setTexture(colorAttachment);
      screenQuad.draw(bloomScreenShader);
    }
  });

  return 0;
}
