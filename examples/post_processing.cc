// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/camera.h>
#include <qrk/framebuffer.h>
#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/vertex_array.h>
#include <qrk/window.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

const char* textureVertexSource = R"SHADER(
#version 460 core
layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexTexCoords;

out vec2 texCoords;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertexPos, 1.0);

  texCoords = vertexTexCoords;
  fragPos = vec3(view * model * vec4(vertexPos, 1.0));
}
)SHADER";

const char* textureFragmentSource = R"SHADER(
#version 460 core
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D texture0;

void main() { fragColor = texture(texture0, texCoords); }
)SHADER";

const char* screenVertexSource = R"SHADER(
#version 460 core
layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec2 vertexTexCoords;

out vec2 texCoords;

void main() {
  gl_Position = vec4(vertexPos, 0.0, 1.0);

  texCoords = vertexTexCoords;
}
)SHADER";

const char* screenFragmentSource = R"SHADER(
#version 460 core
#pragma qrk_include < gamma.frag >
#pragma qrk_include < post_processing.frag >
#pragma qrk_include < window.frag >
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

// TODO: Allow other post_processing methods from command line.
void main() {
  if (qrk_isWindowLeftHalf()) {
    if (qrk_isWindowTopHalf()) {
      fragColor = texture(screenTexture, texCoords);
    } else {
      fragColor = qrk_blurKernel(screenTexture, texCoords);
    }
  } else {
    if (qrk_isWindowTopHalf()) {
      fragColor = qrk_grayscale(texture(screenTexture, texCoords));
    } else {
      fragColor = qrk_edgeKernel(screenTexture, texCoords);
    }
  }
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
)SHADER";

// clang-format off
const float cubeVertices[] = {
    // positions          // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
const float planeVertices[] = {
    // positions          // texture coords
     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

     5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};
const float quadVertices[] = {
    // positions   // texture coords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
// clang-format on

int main() {
  auto win = std::make_shared<qrk::Window>(800, 600, "Post processing");
  win->setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
  win->enableMouseCapture();
  win->setEscBehavior(qrk::EscBehavior::CLOSE);

  auto camera =
      std::make_shared<qrk::Camera>(/* position */ glm::vec3(0.0f, 0.0f, 3.0f));
  auto cameraControls = std::make_shared<qrk::FlyCameraControls>();
  win->bindCamera(camera);
  win->bindCameraControls(cameraControls);

  qrk::Shader mainShader((qrk::ShaderInline(textureVertexSource)),
                         qrk::ShaderInline(textureFragmentSource));
  qrk::Shader screenShader((qrk::ShaderInline(screenVertexSource)),
                           qrk::ShaderInline(screenFragmentSource));

  // TODO: Clean this up to use mesh primitives.
  // Create a VAO for the boxes.
  qrk::VertexArray cubeVarray;
  cubeVarray.loadVertexData(cubeVertices, sizeof(cubeVertices));
  cubeVarray.addVertexAttrib(3, GL_FLOAT);
  cubeVarray.addVertexAttrib(2, GL_FLOAT);
  cubeVarray.finalizeVertexAttribs();

  // Create a VAO for the plane.
  qrk::VertexArray planeVarray;
  planeVarray.loadVertexData(planeVertices, sizeof(planeVertices));
  planeVarray.addVertexAttrib(3, GL_FLOAT);
  planeVarray.addVertexAttrib(2, GL_FLOAT);
  planeVarray.finalizeVertexAttribs();

  // Create a VAO for the screen quad.
  qrk::VertexArray quadVarray;
  quadVarray.loadVertexData(quadVertices, sizeof(quadVertices));
  quadVarray.addVertexAttrib(2, GL_FLOAT);
  quadVarray.addVertexAttrib(2, GL_FLOAT);
  quadVarray.finalizeVertexAttribs();

  // Load textures.
  qrk::Texture cubeTexture =
      qrk::Texture::load("examples/assets/container.jpg");
  qrk::Texture floorTexture = qrk::Texture::load("examples/assets/metal.png");

  // Framebuffer.
  qrk::Framebuffer fb(win->getSize());
  auto colorAttachment = fb.attachTexture(qrk::BufferType::COLOR);
  fb.attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);

  screenShader.addUniformSource(win);

  win->loop([&](float deltaTime) {
    fb.activate();
    fb.clear();

    glm::mat4 view = camera->getViewTransform();
    glm::mat4 projection = camera->getPerspectiveTransform();

    // Setup shader and textures for the framebuffer.
    mainShader.activate();
    mainShader.setMat4("view", view);
    mainShader.setMat4("projection", projection);
    mainShader.setInt("texture0", 0);

    glm::mat4 model;

    // Draw cubes.
    cubeVarray.activate();
    cubeTexture.bindToUnit(0);
    model = glm::translate(glm::mat4(), glm::vec3(-1.0f, 0.0f, -1.0f));
    mainShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::translate(glm::mat4(), glm::vec3(2.0f, 0.0f, 0.0f));
    mainShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Draw floor.
    planeVarray.activate();
    floorTexture.bindToUnit(0);
    mainShader.setMat4("model", glm::mat4());
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVarray.deactivate();

    fb.deactivate();

    // Finally, draw to the screen based on the framebuffer contents.
    screenShader.updateUniforms();
    screenShader.activate();
    screenShader.setInt("screenTexture", 0);
    quadVarray.activate();
    glBindTexture(GL_TEXTURE_2D, colorAttachment.id);
    win->disableDepthTest();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    win->enableDepthTest();
  });

  return 0;
}
