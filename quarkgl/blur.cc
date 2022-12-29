#include <qrk/blur.h>

namespace qrk {

PingPongPass::PingPongPass(int width, int height)
    : buffers_{Framebuffer(width, height), Framebuffer(width, height)} {
  attachments_[0] = buffers_[0].attachTexture(BufferType::COLOR_HDR_ALPHA);
  attachments_[1] = buffers_[1].attachTexture(BufferType::COLOR_HDR_ALPHA);
}

void PingPongPass::multipassDraw(Texture source, Shader& shader, int passes,
                                 std::function<void()> callback,
                                 TextureRegistry* textureRegistry) {
  // For the very first iteration, we render from the source to buffer0.
  Framebuffer* currentFb = &buffers_[0];
  Texture currentTexture = source;

  const int totalIterations = passes * 2;
  for (int i = 0; i < totalIterations; i++) {
    currentFb->activate();

    callback();
    screenQuad_.setTexture(currentTexture);
    screenQuad_.draw(shader, textureRegistry);

    // For every subsequent iteration, we switch our target buffer, and then
    // render from the opposing attachment.
    const int nextFbIdx = (i + 1) % 2;
    currentFb = &buffers_[nextFbIdx];
    currentTexture = attachments_[nextFbIdx == 0 ? 1 : 0].asTexture();

    currentFb->deactivate();
  }
}

GaussianBlurShader::GaussianBlurShader()
    : ScreenShader(ShaderPath("quarkgl/shaders/builtin/gaussian_blur.frag")) {}

}  // namespace qrk