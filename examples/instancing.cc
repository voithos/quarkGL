// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/quarkgl.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

int main() {
  qrk::Window win(800, 600, "Instancing");
  win.setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE);
  win.setMouseButtonBehavior(qrk::MouseButtonBehavior::CAPTURE_MOUSE);

  auto camera = std::make_shared<qrk::Camera>(
      /* position */ glm::vec3(0.0f, 0.0f, 10.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win.bindCamera(camera);
  win.bindCameraControls(cameraControls);

  qrk::Shader mainShader(qrk::ShaderPath("examples/shaders/model.vert"),
                         qrk::ShaderPath("examples/shaders/phong.frag"));

  mainShader.setFloat("material.shininess", 32.0f);
  mainShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  mainShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  mainShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  qrk::Shader instancedShader(
      qrk::ShaderPath("examples/shaders/instancing.vert"),
      qrk::ShaderPath("examples/shaders/phong.frag"));

  instancedShader.setFloat("material.shininess", 32.0f);
  instancedShader.setFloat("material.emissionAttenuation.constant", 1.0f);
  instancedShader.setFloat("material.emissionAttenuation.linear", 0.09f);
  instancedShader.setFloat("material.emissionAttenuation.quadratic", 0.032f);

  // Create light registry and add lights.
  auto registry = std::make_shared<qrk::LightRegistry>();
  mainShader.addUniformSource(registry);
  instancedShader.addUniformSource(registry);

  auto directionalLight =
      std::make_shared<qrk::DirectionalLight>(glm::vec3(-0.2f, -1.0f, -0.3f));
  directionalLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(directionalLight);

  auto pointLight =
      std::make_shared<qrk::PointLight>(glm::vec3(1.2f, 1.0f, 2.0f));
  pointLight->setSpecular(glm::vec3(0.5f, 0.5f, 0.5f));
  registry->addLight(pointLight);

  // Generate asteroid distribution.
  int rockCount = 8000;
  glm::mat4 modelTransforms[rockCount];
  std::mt19937 gen(42);
  float radius = 15.0f;
  float offset = 2.5f;
  for (int i = 0; i < rockCount; i++) {
    glm::mat4 model;

    // Translate: displace along circle with offset radius.
    float angle =
        static_cast<float>(i) / static_cast<float>(rockCount) * 360.0f;
    float displacement =
        (gen() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
    float x = sin(angle) * radius + displacement;
    displacement =
        (gen() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
    float y = displacement * 0.4f;
    displacement =
        (gen() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
    float z = cos(angle) * radius + displacement;
    model = glm::translate(model, glm::vec3(x, y, z));

    // Scale: scale between 0.05 and 0.25.
    float scale = (gen() % 5) / 100.0f + 0.01;
    model = glm::scale(model, glm::vec3(scale));

    // Rotate: add random rotation.
    float rotAngle = gen() % 360;
    model =
        glm::rotate(model, glm::radians(rotAngle), glm::vec3(0.4f, 0.6f, 0.8f));

    modelTransforms[i] = model;
  }

  // Load models.
  qrk::Model planet("examples/assets/planet/planet.obj");
  qrk::Model plainRock("examples/assets/rock/rock.obj");
  qrk::Model rock("examples/assets/rock/rock.obj",
                  /*instanceCount=*/rockCount);
  rock.loadInstanceModels(modelTransforms, rockCount);

  win.enableFaceCull();
  win.loop([&](float deltaTime) {
    glm::mat4 view = camera->getViewTransform();
    glm::mat4 projection = camera->getProjectionTransform();
    registry->applyViewTransform(view);

    // Setup shader and lights.
    mainShader.activate();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);
    mainShader.updateUniforms();

    // Draw planet.
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(50.f), glm::vec3(1.0f, 0.0f, 0.0f));
    mainShader.setMat4("model", model);

    planet.draw(mainShader);

    // Draw rocks.
    instancedShader.activate();
    instancedShader.setMat4("view", view);
    instancedShader.setMat4("projection", projection);
    registry->applyViewTransform(view);
    instancedShader.updateUniforms();

    rock.draw(instancedShader);
  });

  return 0;
}
