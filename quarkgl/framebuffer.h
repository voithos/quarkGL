#ifndef QUARKGL_FRAMEBUFFER_H_
#define QUARKGL_FRAMEBUFFER_H_

#include <string>
#include <vector>

#include <qrk/shared.h>
#include <qrk/window.h>

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

inline const GLenum bufferTypeToAttachmentType(BufferType type) {
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

inline const GLenum bufferTypeToInternalFormat(BufferType type) {
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

inline const GLenum bufferTypeToFormat(BufferType type) {
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

inline const GLenum bufferTypeToInternalDataType(BufferType type) {
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

class Framebuffer {
 private:
  unsigned int fbo_ = 0;
  int width_;
  int height_;
  std::vector<Attachment> attachments_;

  Attachment saveAttachment(unsigned int id, AttachmentTarget target);

 public:
  Framebuffer(int width, int height);
  Framebuffer(ScreenSize size) : Framebuffer(size.width, size.height) {}
  ~Framebuffer();

  void activate();
  void deactivate();

  ScreenSize getSize();

  Attachment attachTexture(BufferType type);
  Attachment attachRenderbuffer(BufferType type);
};

}  // namespace qrk

#endif
