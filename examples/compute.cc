// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/exceptions.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/window.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int main() {
  qrk::Window win(512, 512, "Compute shader", /* fullscreen */ false,
                  /* samples */ 4);
  win.setClearColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
  // rm win.enableMouseCapture();
  win.setEscBehavior(qrk::EscBehavior::CLOSE);

  qrk::ComputeShader computeShader(qrk::ShaderPath("examples/compute.comp"));
  qrk::Texture computeTexture = qrk::Texture::create(512, 512, GL_RGBA32F);
  qrk::ScreenQuadMesh screenQuad(computeTexture);
  qrk::ScreenQuadShader screenShader;

  win.loop([&](float deltaTime) {
    // Run the compute shader.
    computeShader.dispatchToTexture(computeTexture);

    // Draw the results.
    screenQuad.draw(screenShader);
  });

  return 0;
}
