#ifndef QUARKGL_FRAMEBUFFER_H_
#define QUARKGL_FRAMEBUFFER_H_

#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/window.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace qrk {

class FramebufferException : public QuarkException {
  using QuarkException::QuarkException;
};

enum class AttachmentTarget {
  TEXTURE,
  RENDERBUFFER,
};

enum class BufferType {
  COLOR = 0,
  // HDR color attachment, allowing color values to exceed 1.0
  COLOR_HDR,
  // SNORM attachment, only allowing numbers in the range [-1, 1] but with
  // greater precision.
  COLOR_SNORM,
  COLOR_ALPHA,
  COLOR_HDR_ALPHA,
  COLOR_SNORM_ALPHA,
  COLOR_CUBEMAP_HDR,
  COLOR_CUBEMAP_HDR_ALPHA,
  GRAYSCALE,
  DEPTH,
  STENCIL,
  DEPTH_AND_STENCIL,
};

class Attachment {
 public:
  // TODO: Make these private.
  unsigned int id;
  int width;
  int height;
  int numMips;
  AttachmentTarget target;
  BufferType type;
  // Attachment index. Only applies for color buffers.
  int colorAttachmentIndex;
  TextureType textureType;

  Texture asTexture();

  // TODO: Replace with Texture?
};

inline const GLenum bufferTypeToGlAttachmentType(BufferType type,
                                                 int attachmentIndex) {
  switch (type) {
    case BufferType::COLOR:
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_SNORM:
    case BufferType::COLOR_ALPHA:
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::COLOR_SNORM_ALPHA:
    case BufferType::COLOR_CUBEMAP_HDR:
    case BufferType::COLOR_CUBEMAP_HDR_ALPHA:
    case BufferType::GRAYSCALE:
      return GL_COLOR_ATTACHMENT0 + attachmentIndex;
    case BufferType::DEPTH:
      return GL_DEPTH_ATTACHMENT;
    case BufferType::STENCIL:
      return GL_STENCIL_ATTACHMENT;
    case BufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH_STENCIL_ATTACHMENT;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
}

inline const GLenum bufferTypeToGlInternalFormat(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
      return GL_RGB8;
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_CUBEMAP_HDR:
      return GL_RGB16F;
    case BufferType::COLOR_SNORM:
      return GL_RGB16_SNORM;
    case BufferType::COLOR_ALPHA:
      return GL_RGBA8;
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_RGBA16F;
    case BufferType::COLOR_SNORM_ALPHA:
      return GL_RGBA16_SNORM;
    case BufferType::GRAYSCALE:
      return GL_R8;
    case BufferType::DEPTH:
      return GL_DEPTH_COMPONENT32F;
    case BufferType::STENCIL:
      return GL_STENCIL_INDEX8;
    case BufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH24_STENCIL8;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
}

inline const GLenum bufferTypeToGlFormat(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_SNORM:
    case BufferType::COLOR_CUBEMAP_HDR:
      return GL_RGB;
    case BufferType::COLOR_ALPHA:
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::COLOR_SNORM_ALPHA:
    case BufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_RGBA;
    case BufferType::GRAYSCALE:
      return GL_RED;
    case BufferType::DEPTH:
      return GL_DEPTH_COMPONENT;
    case BufferType::STENCIL:
      return GL_STENCIL_INDEX;
    case BufferType::DEPTH_AND_STENCIL:
      return GL_DEPTH_STENCIL;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
}

inline const GLenum bufferTypeToGlDataType(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
    case BufferType::COLOR_ALPHA:
      return GL_UNSIGNED_BYTE;
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::COLOR_SNORM:
    case BufferType::COLOR_SNORM_ALPHA:
    case BufferType::COLOR_CUBEMAP_HDR:
    case BufferType::COLOR_CUBEMAP_HDR_ALPHA:
      return GL_FLOAT;
    case BufferType::GRAYSCALE:
      return GL_UNSIGNED_BYTE;
    case BufferType::DEPTH:
      return GL_FLOAT;
    case BufferType::STENCIL:
      return GL_UNSIGNED_BYTE;
    case BufferType::DEPTH_AND_STENCIL:
      return GL_UNSIGNED_INT_24_8;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
}

// Represents a generated framebuffer
class Framebuffer {
 public:
  Framebuffer(int width, int height, int samples = 0);
  explicit Framebuffer(ImageSize size, int samples = 0)
      : Framebuffer(size.width, size.height, samples) {}
  virtual ~Framebuffer();

  // Activates the current framebuffer. Optionally specify a mipmap level to
  // draw to, and a cubemap face (0 means GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc).
  void activate(int mipLevel = 0, int cubemapFace = -1);
  // Deactivates the current framebuffer (and activates the default screen
  // framebuffer).
  void deactivate();

  glm::vec4 getClearColor() { return clearColor_; }
  void setClearColor(glm::vec4 color) { clearColor_ = color; }
  void clear();

  ImageSize getSize();

  Attachment attachTexture(BufferType type);
  Attachment attachTexture(BufferType type, const TextureParams& params);
  Attachment attachRenderbuffer(BufferType type);

  // Returns the first texture attachment of the given type.
  Attachment getTexture(BufferType type);
  // Returns the first renderbuffer attachment of the given type.
  Attachment getRenderbuffer(BufferType type);

  // Copies the framebuffer to the target.
  // TODO: Swap out GLenum for qrk type.
  void blit(Framebuffer& target, GLenum bits);
  // Copies the framebuffer to the default framebuffer.
  void blitToDefault(GLenum type);

  void enableAlphaBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
  }
  void disableAlphaBlending() { glDisable(GL_BLEND); }

  void enableAdditiveBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);
  }
  void disableAdditiveBlending() { glDisable(GL_BLEND); }

 private:
  unsigned int fbo_ = 0;
  int width_;
  int height_;
  int samples_;
  std::vector<Attachment> attachments_;

  bool hasColorAttachment_ = false;
  int numColorAttachments_ = 0;
  bool hasDepthAttachment_ = false;
  bool hasStencilAttachment_ = false;
  glm::vec4 clearColor_ = DEFAULT_CLEAR_COLOR;

  Attachment saveAttachment(unsigned int id, int numMips,
                            AttachmentTarget target, BufferType type,
                            int colorAttachmentIndex, TextureType textureType);
  Attachment getAttachment(AttachmentTarget target, BufferType type);
  void checkFlags(BufferType type);
  void updateFlags(BufferType type);
  // Updates the draw and read buffers based on the current flags.
  void updateBufferSources();
};

}  // namespace qrk

#endif
