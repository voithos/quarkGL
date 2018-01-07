#include <glad/glad.h>

#include <qrk/framebuffer.h>

namespace qrk {

Framebuffer::Framebuffer(int width, int height) {
  width_ = width;
  height_ = height;
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

  // Generate texture based on given buffer type.
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  GLenum internalFormat = bufferTypeToInternalFormat(type);
  GLenum format = bufferTypeToFormat(type);
  GLenum dataType = bufferTypeToInternalDataType(type);

  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, internalFormat, width_,
               height_, 0, format, dataType, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Attach the texture to the framebuffer.
  GLenum attachmentType = bufferTypeToAttachmentType(type);
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, GL_TEXTURE_2D, texture,
                         /* mipmap level */ 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  deactivate();

  return saveAttachment(texture, AttachmentTarget::TEXTURE);
}

Attachment Framebuffer::attachRenderbuffer(BufferType type) {
  activate();

  // Create and configure renderbuffer.
  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);

  GLenum internalFormat = bufferTypeToInternalFormat(type);

  glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width_, height_);

  // Attach the renderbuffer to the framebuffer.
  GLenum attachmentType = bufferTypeToAttachmentType(type);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER,
                            rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  deactivate();

  return saveAttachment(rbo, AttachmentTarget::RENDERBUFFER);
}

Attachment Framebuffer::saveAttachment(unsigned int id,
                                       AttachmentTarget target) {
  Attachment attachment = {.id = id, .target = target};
  attachments_.push_back(attachment);
  return attachment;
}

}  // namespace qrk
