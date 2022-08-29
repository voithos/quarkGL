#ifndef QUARKGL_FRAMEBUFFER_H_
#define QUARKGL_FRAMEBUFFER_H_

#include <qrk/shader.h>
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

struct Attachment {
  unsigned int id;
  AttachmentTarget target;
};

enum class BufferType {
  COLOR,
  DEPTH,
  STENCIL,
  DEPTH_AND_STENCIL,
};

inline const GLenum bufferTypeToGlAttachmentType(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
      // TODO: Support color attachments >0.
      return GL_COLOR_ATTACHMENT0;
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
      return GL_RGB;
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
      return GL_RGB;
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

inline const GLenum bufferTypeToGlInternalDataType(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
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
  explicit Framebuffer(ScreenSize size, int samples = 0)
      : Framebuffer(size.width, size.height, samples) {}
  ~Framebuffer();

  // Activates the current framebuffer.
  void activate();
  // Deactivates the current framebuffer (and activates the default screen
  // framebuffer).
  void deactivate();

  glm::vec4 getClearColor() { return clearColor_; }
  void setClearColor(glm::vec4 color) { clearColor_ = color; }
  void clear();

  ScreenSize getSize();

  Attachment attachTexture(BufferType type);
  Attachment attachRenderbuffer(BufferType type);

  // TODO: Add support for glBlitFramebuffer, when multisampled.

 private:
  unsigned int fbo_ = 0;
  int width_;
  int height_;
  int samples_;
  std::vector<Attachment> attachments_;

  bool hasColorAttachment_ = false;
  bool hasDepthAttachment_ = false;
  bool hasStencilAttachment_ = false;
  glm::vec4 clearColor_ = DEFAULT_CLEAR_COLOR;

  Attachment saveAttachment(unsigned int id, AttachmentTarget target);
  void updateFlags(BufferType type);
  // Updates the draw and read buffers based on the current flags.
  void updateBufferSource();
};

}  // namespace qrk

#endif
