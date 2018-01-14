#include <iostream>

// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qrk/camera.h>
#include <qrk/light.h>
#include <qrk/model.h>
#include <qrk/shader.h>
#include <qrk/vertex_array.h>
#include <qrk/window.h>

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// TODO: Abstract first-person camera controls.
float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool initialMouse = true;

qrk::Camera camera(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.zoom(yoffset);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  if (initialMouse) {
    lastX = xpos;
    lastY = ypos;
    initialMouse = false;
  }
  float xoffset = xpos - lastX;
  // Reversed since y-coordinates range from bottom to top.
  float yoffset = lastY - ypos;
  lastX = xpos;
  lastY = ypos;

  camera.rotate(xoffset, yoffset);
}

void processInput(GLFWwindow* window, float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::RIGHT, deltaTime);
  }
}

// clang-format off
float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
// clang-format on

int main() {
  qrk::Window win(SCREEN_WIDTH, SCREEN_HEIGHT, "Model render");
  win.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::TOGGLE_MOUSE_CAPTURE);
  camera.setAspectRatio(win.getSize());
  auto window = win.getGlfwRef();

  // TODO: Clean these calls up by moving them into qrk::Window.
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetCursorPosCallback(window, mouseCallback);

  qrk::Shader mainShader("examples/phong.vert", "examples/phong.frag");

  mainShader.setFloat("material.shininess", 8.0f);
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

  qrk::Shader lampShader("examples/phong.vert", "examples/lamp.frag");

  // Create a VAO for the light.
  // TODO: Make this into a sphere.
  qrk::VertexArray lightVarray;
  lightVarray.loadVertexData(vertices, sizeof(vertices));
  lightVarray.addVertexAttrib(3, GL_FLOAT);
  lightVarray.addVertexAttrib(3, GL_FLOAT);
  lightVarray.addVertexAttrib(2, GL_FLOAT);
  lightVarray.finalizeVertexAttribs();

  // Load model.
  qrk::Model nanosuit("examples/nanosuit/nanosuit.obj");

  win.enableCulling();
  win.loop([&](float deltaTime) {
    processInput(window, deltaTime);

    glm::mat4 view = camera.getViewTransform();
    glm::mat4 projection = camera.getPerspectiveTransform();

    // Setup shader and lights.
    mainShader.activate();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);

    registry->applyViewTransform(view);
    mainShader.updateUniforms();

    // Draw main model.
    // Translate the model down so it's in the center.
    glm::mat4 model =
        glm::translate(glm::mat4(), glm::vec3(0.0f, -1.75f, 0.0f));
    // Scale it down, since it's too big.
    model = glm::scale(model, glm::vec3(0.2f));
    mainShader.setMat4("model", model);

    nanosuit.draw(mainShader);

    // Draw light source.
    glm::mat4 lightModel =
        glm::translate(glm::mat4(), pointLight->getPosition());
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));
    lampShader.activate();
    lampShader.setMat4("model", lightModel);
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);
    lightVarray.activate();
    glDrawArrays(GL_TRIANGLES, 0, 36);
  });

  return 0;
}
