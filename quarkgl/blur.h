#ifndef QUARKGL_BLUR_H_
#define QUARKGL_BLUR_H_

#include <qrk/framebuffer.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/texture_registry.h>

namespace qrk {

class PingPongPass {
 public:
  PingPongPass(int width, int height);
  explicit PingPongPass(ImageSize size)
      : PingPongPass(size.width, size.height) {}
  virtual ~PingPongPass() = default;

  // Draws using the given shader multiple times, using the configured source
  // first and then ping-ponging between internal framebuffers. Passes represent
  // a single ping-pong cycle (so N passes means N*2 draw calls). The callback
  // should configure or update any needed shader uniforms, etc. After drawing,
  // you can call getOutput() to get the resulting texture.
  void multipassDraw(Texture source, Shader& shader, int passes,
                     std::function<void()> callback,
                     TextureRegistry* textureRegistry = nullptr);

  Texture getOutput() {
    // The final result is always stored in the 2nd FBO.
    return attachments_[1].asTexture();
  }

 private:
  ScreenQuadMesh screenQuad_;
  // Ping-pong buffers and attachments.
  Framebuffer buffers_[2];
  Attachment attachments_[2];
};

// A shader that performs a single-pass gaussian blur.
class GaussianBlurShader : public ScreenShader {
 public:
  GaussianBlurShader();

  // Whether to perform a horizontal blur. If false, performs a vertical blur.
  void setHorizontal(bool horizontal) {
    horizontal_ = horizontal;
    setBool("horizontal", horizontal_);
  }
  bool getHorizontal() { return horizontal_; }

 private:
  bool horizontal_ = false;
};

}  // namespace qrk

#endif