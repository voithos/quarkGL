#include <glad/glad.h>

#include <qrk/framebuffer.h>

namespace qrk {

Framebuffer::Framebuffer(int width, int height, int samples)
    : width_(width), height_(height), samples_(samples) {
  glGenFramebuffers(1, &fbo_);
}

Framebuffer::~Framebuffer() { glDeleteFramebuffers(1, &fbo_); }

void Framebuffer::activate() { glBindFramebuffer(GL_FRAMEBUFFER, fbo_); }

void Framebuffer::deactivate() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

ScreenSize Framebuffer::getSize() {
  ScreenSize size = {.width = width_, .height = height_};
  return size;
}

Attachment Framebuffer::attachTexture(BufferType type) {
  activate();

  GLenum textureTarget = samples_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

  // Generate texture based on given buffer type.
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(textureTarget, texture);

  GLenum internalFormat = bufferTypeToGlInternalFormat(type);
  GLenum format = bufferTypeToGlFormat(type);
  GLenum dataType = bufferTypeToGlInternalDataType(type);

  if (samples_) {
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_, internalFormat,
                            width_, height_,
                            /* fixedsamplelocations */ GL_TRUE);
  } else {
    glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, internalFormat, width_,
                 height_, 0, format, dataType, nullptr);
  }
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Attach the texture to the framebuffer.
  GLenum attachmentType = bufferTypeToGlAttachmentType(type);
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget, texture,
                         /* mipmap level */ 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  glBindTexture(textureTarget, 0);
  deactivate();

  updateFlags(type);
  return saveAttachment(texture, AttachmentTarget::TEXTURE);
}

Attachment Framebuffer::attachRenderbuffer(BufferType type) {
  activate();

  // Create and configure renderbuffer.
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);

  GLenum internalFormat = bufferTypeToGlInternalFormat(type);

  if (samples_) {
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples_, internalFormat,
                                     width_, height_);
  } else {
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width_, height_);
  }

  // Attach the renderbuffer to the framebuffer.
  GLenum attachmentType = bufferTypeToGlAttachmentType(type);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER,
                            rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  deactivate();

  updateFlags(type);
  return saveAttachment(rbo, AttachmentTarget::RENDERBUFFER);
}

Attachment Framebuffer::saveAttachment(unsigned int id,
                                       AttachmentTarget target) {
  Attachment attachment = {.id = id, .target = target};
  attachments_.push_back(attachment);
  return attachment;
}

void Framebuffer::updateFlags(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
      hasColorAttachment_ = true;
      return;
    case BufferType::DEPTH:
      hasDepthAttachment_ = true;
      return;
    case BufferType::STENCIL:
      hasStencilAttachment_ = true;
      return;
    case BufferType::DEPTH_AND_STENCIL:
      hasDepthAttachment_ = true;
      hasStencilAttachment_ = true;
      return;
  }
  throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_BUFFER_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
}

void Framebuffer::clear() {
  glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, clearColor_.a);

  GLbitfield clearBits = 0;
  if (hasColorAttachment_) {
    clearBits |= GL_COLOR_BUFFER_BIT;
  }
  if (hasDepthAttachment_) {
    clearBits |= GL_DEPTH_BUFFER_BIT;
  }
  if (hasStencilAttachment_) {
    clearBits |= GL_STENCIL_BUFFER_BIT;
  }
  glClear(clearBits);
}

}  // namespace qrk
