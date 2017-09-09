#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qrk/camera.h>
#include <qrk/shader.h>
#include <qrk/vertex_array.h>

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

float lastX = SCREEN_WIDTH / 2.0f;
float lastY = SCREEN_HEIGHT / 2.0f;
bool initialMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 worldLightPos(1.2f, 1.0f, 2.0f);

qrk::Camera camera(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
  camera.processMouseScroll(yoffset);
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

  camera.processMouseMove(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.processKeyboard(qrk::CameraDirection::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.processKeyboard(qrk::CameraDirection::LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.processKeyboard(qrk::CameraDirection::BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.processKeyboard(qrk::CameraDirection::RIGHT, deltaTime);
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

glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f),
  glm::vec3( 2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3( 2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3( 1.3f, -2.0f, -2.5f),
  glm::vec3( 1.5f,  2.0f, -2.5f),
  glm::vec3( 1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
};
// clang-format on

unsigned int createTexture(const char* filePath) {
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set texture-wrapping/filtering options.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, numChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(filePath, &width, &height, &numChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0,
                 /* texture format */ GL_RGB, width, height, 0,
                 /* tex data format */ numChannels == 3 ? GL_RGB : GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "ERROR::TEXTURE::LOAD_FAILED\n" << filePath << std::endl;
  }
  stbi_image_free(data);

  return texture;
}

int main() {
  qrk::init();

  GLFWwindow* window =
      glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "LearnOpenGL",
                       /* monitor */ NULL, /* share */ NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetScrollCallback(window, scrollCallback);
  glfwSetCursorPosCallback(window, mouseCallback);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // Load box texture.
  glActiveTexture(GL_TEXTURE0);
  createTexture("examples/container2.png");
  glActiveTexture(GL_TEXTURE1);
  createTexture("examples/container2_specular.png");
  glActiveTexture(GL_TEXTURE2);
  createTexture("examples/matrix.jpg");

  qrk::Shader mainShader("examples/vertex.glsl", "examples/fragment.glsl");
  // These correspond to the texture numbers above.
  mainShader.setInt("material.diffuse", 0);
  mainShader.setInt("material.specular", 1);
  mainShader.setInt("material.emission", 2);
  mainShader.setFloat("material.shininess", 64.0f);

  mainShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
  mainShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
  mainShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

  mainShader.setFloat("light.attenuation.constant", 1.0f);
  mainShader.setFloat("light.attenuation.linear", 0.09f);
  mainShader.setFloat("light.attenuation.quadratic", 0.032f);

  qrk::Shader lampShader("examples/vertex.glsl",
                         "examples/light_fragment.glsl");

  qrk::VertexArray varray;
  varray.loadVertexData(vertices, sizeof(vertices));
  varray.addVertexAttrib(3, GL_FLOAT);
  varray.addVertexAttrib(3, GL_FLOAT);
  varray.addVertexAttrib(2, GL_FLOAT);
  varray.finalizeVertexAttribs();

  // Create another VAO for the light, reusing the VBO.
  qrk::VertexArray lightVarray;
  lightVarray.use();
  glBindBuffer(GL_ARRAY_BUFFER, varray.getVbo());
  lightVarray.addVertexAttrib(3, GL_FLOAT);
  lightVarray.addVertexAttrib(3, GL_FLOAT);
  lightVarray.addVertexAttrib(2, GL_FLOAT);
  lightVarray.finalizeVertexAttribs();

  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    mainShader.setFloat("time", currentFrame);

    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(
        glm::radians(camera.getFov()),
        SCREEN_WIDTH / static_cast<float>(SCREEN_HEIGHT), 0.1f, 100.0f);

    // Draw main cube.
    mainShader.use();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);
    glm::vec3 viewLightPos = glm::vec3(view * glm::vec4(worldLightPos, 1.0));
    mainShader.setVec3("light.position", viewLightPos);

    varray.use();
    for (unsigned int i = 0;
         i < sizeof(cubePositions) / sizeof(cubePositions[0]); i++) {
      // Calculate the model matrix.
      glm::mat4 model = glm::translate(glm::mat4(), cubePositions[i]);
      float angle = 20.0f * i;
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      mainShader.setMat4("model", model);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Draw light source.
    glm::mat4 model = glm::translate(glm::mat4(), worldLightPos);
    model = glm::scale(model, glm::vec3(0.2f));
    lampShader.use();
    lampShader.setMat4("model", model);
    lampShader.setMat4("view", view);
    lampShader.setMat4("projection", projection);
    lightVarray.use();
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  qrk::terminate();
  return 0;
}
