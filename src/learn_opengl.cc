#include "shader.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <stb/stb_image.h>
#include <string>

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

unsigned int createRectangle() {
  // clang-format off
  float vertices[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
  };
  unsigned int indices[] = {
    0, 1, 2,
    0, 2, 3
  };
  // clang-format on

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  int stride = 8;
  glVertexAttribPointer(/* layout position */ 0, /* size */ 3,
                        /* type */ GL_FLOAT,
                        /* normalized */ GL_FALSE,
                        /* stride */ stride * sizeof(float),
                        /* offset */ (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(/* layout position */ 1, /* size */ 3,
                        /* type */ GL_FLOAT,
                        /* normalized */ GL_FALSE,
                        /* stride */ stride * sizeof(float),
                        /* offset */ (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(/* layout position */ 2, /* size */ 2,
                        /* type */ GL_FLOAT,
                        /* normalized */ GL_FALSE,
                        /* stride */ stride * sizeof(float),
                        /* offset */ (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
  return VAO;
}

unsigned int createTexture(const char *filePath) {
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
  unsigned char *data = stbi_load(filePath, &width, &height, &numChannels, 0);
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
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL",
                                        /* monitor */ NULL, /* share */ NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  Shader mainShader("src/vertex.glsl", "src/fragment.glsl");

  unsigned int VAO = createRectangle();
  unsigned int texture0 = createTexture("src/container.jpg");
  unsigned int texture1 = createTexture("src/awesomeface.png");

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    mainShader.use();
    mainShader.setInt("texture0", 0);
    mainShader.setInt("texture1", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, /* offset */ 0);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
