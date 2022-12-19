#include <glad/glad.h>
#include <qrk/framebuffer.h>

namespace qrk {

Texture Attachment::asTexture() {
  if (target != AttachmentTarget::TEXTURE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INVALID_ATTACHMENT_TARGET");
  }
  Texture texture;
  texture.id_ = id;
  return texture;
}

Framebuffer::Framebuffer(int width, int height, int samples)
    : width_(width), height_(height), samples_(samples) {
  glGenFramebuffers(1, &fbo_);
}

Framebuffer::~Framebuffer() { glDeleteFramebuffers(1, &fbo_); }

void Framebuffer::activate() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
  glViewport(0, 0, width_, height_);
}

void Framebuffer::deactivate() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

ScreenSize Framebuffer::getSize() {
  ScreenSize size = {.width = width_, .height = height_};
  return size;
}

Attachment Framebuffer::attachTexture(BufferType type) {
  TextureParams params = {.filtering = TextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return attachTexture(type, params);
}

Attachment Framebuffer::attachTexture(BufferType type,
                                      const TextureParams& params) {
  checkFlags(type);
  activate();

  GLenum textureTarget = samples_ ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

  // Generate texture based on given buffer type.
  // TODO: This does the same thing as the Texture class - should it use that
  // instead?
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(textureTarget, texture);

  GLenum internalFormat = bufferTypeToGlInternalFormat(type);
  GLenum format = bufferTypeToGlFormat(type);
  GLenum dataType = bufferTypeToGlInternalDataType(type);

  if (samples_) {
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples_, internalFormat,
                            width_, height_,
                            /*fixedsamplelocations=*/GL_TRUE);
  } else {
    glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, internalFormat, width_,
                 height_, 0, format, dataType, nullptr);
  }

  Texture::applyParams(params);

  // Attach the texture to the framebuffer.
  GLenum attachmentType =
      bufferTypeToGlAttachmentType(type, numColorAttachments_);
  glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, textureTarget, texture,
                         /* mipmap level */ 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  updateFlags(type);
  updateBufferSources();

  glBindTexture(textureTarget, 0);
  deactivate();

  return saveAttachment(texture, AttachmentTarget::TEXTURE);
}

Attachment Framebuffer::attachRenderbuffer(BufferType type) {
  checkFlags(type);
  activate();

  // Create and configure renderbuffer.
  // Renderbuffers are similar to textures, but they generally cannot be read
  // from easily. In exchange, their render data is stored in a native format,
  // so they are perfect for use cases that require writing (such as the final
  // frame, or depth/stencil attachments).
  // TODO: Pull out into a renderbuffer class?
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
  GLenum attachmentType =
      bufferTypeToGlAttachmentType(type, numColorAttachments_);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentType, GL_RENDERBUFFER,
                            rbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw FramebufferException("ERROR::FRAMEBUFFER::INCOMPLETE");
  }

  updateFlags(type);
  updateBufferSources();

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  deactivate();

  return saveAttachment(rbo, AttachmentTarget::RENDERBUFFER);
}

void Framebuffer::blit(Framebuffer& target, GLenum bits) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.fbo_);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, bits,
                    GL_NEAREST);
  deactivate();
}

void Framebuffer::blitToDefault(GLenum bits) {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, bits,
                    GL_NEAREST);
  deactivate();
}

Attachment Framebuffer::saveAttachment(unsigned int id,
                                       AttachmentTarget target) {
  Attachment attachment = {.id = id, .target = target};
  attachments_.push_back(attachment);
  return attachment;
}

void Framebuffer::checkFlags(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_ALPHA:
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::GRAYSCALE:
      // Multiple color attachments OK.
      return;
    case BufferType::DEPTH:
      if (hasDepthAttachment_) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(type)));
      }
      return;
    case BufferType::STENCIL:
      if (hasStencilAttachment_) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(type)));
      }
      return;
    case BufferType::DEPTH_AND_STENCIL:
      if (hasDepthAttachment_ || hasStencilAttachment_) {
        throw FramebufferException(
            "ERROR::FRAMEBUFFER::BUFFER_TYPE_ALREADY_IN_USE\n" +
            std::to_string(static_cast<int>(type)));
      }
      return;
  }
}

void Framebuffer::updateFlags(BufferType type) {
  switch (type) {
    case BufferType::COLOR:
    case BufferType::COLOR_HDR:
    case BufferType::COLOR_ALPHA:
    case BufferType::COLOR_HDR_ALPHA:
    case BufferType::GRAYSCALE:
      hasColorAttachment_ = true;
      numColorAttachments_++;
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

void Framebuffer::updateBufferSources() {
  if (hasColorAttachment_) {
    if (numColorAttachments_ == 1) {
      glDrawBuffer(GL_COLOR_ATTACHMENT0);
    } else {
      std::vector<unsigned int> attachments;
      for (int i = 0; i < numColorAttachments_; i++) {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
      }
      glDrawBuffers(numColorAttachments_, attachments.data());
    }
    // Always read from attachment 0.
    glReadBuffer(GL_COLOR_ATTACHMENT0);
  } else {
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
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
