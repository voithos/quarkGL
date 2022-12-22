#include <glad/glad.h>
#include <qrk/texture.h>
#include <stb/stb_image.h>

#include <glm/gtc/type_ptr.hpp>

namespace qrk {

int calculateNumMips(int width, int height) {
  return 1 + static_cast<int>(std::floor(std::log2(std::max(width, height))));
}

ImageSize calculateNextMip(const ImageSize& mipSize) {
  return ImageSize{.width = std::max(mipSize.width / 2, 1),
                   .height = std::max(mipSize.height / 2, 1)};
}

Texture Texture::load(const char* path, bool isSRGB) {
  TextureParams params = {.filtering = TextureFiltering::ANISOTROPIC,
                          .wrapMode = TextureWrapMode::REPEAT};
  return load(path, isSRGB, params);
}

Texture Texture::load(const char* path, bool isSRGB,
                      const TextureParams& params) {
  Texture texture;

  stbi_set_flip_vertically_on_load(params.flipVerticallyOnLoad);
  unsigned char* data = stbi_load(path, &texture.width_, &texture.height_,
                                  &texture.numChannels_, 0);

  if (data == nullptr) {
    stbi_image_free(data);
    throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + std::string(path));
  }

  GLenum dataFormat;
  if (texture.numChannels_ == 1) {
    texture.internalFormat_ = GL_RED;
    dataFormat = GL_RED;
  } else if (texture.numChannels_ == 3) {
    texture.internalFormat_ = isSRGB ? GL_SRGB : GL_RGB;
    dataFormat = GL_RGB;
  } else if (texture.numChannels_ == 4) {
    texture.internalFormat_ = isSRGB ? GL_SRGB_ALPHA : GL_RGBA;
    dataFormat = GL_RGBA;
  } else {
    stbi_image_free(data);
    throw TextureException(
        "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
        "Texture '" +
        std::string(path) + "' contained unsupported number of channels: " +
        std::to_string(texture.numChannels_));
  }

  glGenTextures(1, &texture.id_);
  glBindTexture(GL_TEXTURE_2D, texture.id_);

  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, texture.internalFormat_,
               texture.width_, texture.height_, 0,
               /* tex data format */ dataFormat, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  // Set texture-wrapping/filtering options.
  applyParams(params);

  stbi_image_free(data);

  return texture;
}

Texture Texture::loadCubemap(std::vector<std::string> faces) {
  if (faces.size() != 6) {
    throw TextureException(
        "ERROR::TEXTURE::INVALID_ARGUMENT\nMust pass exactly 6 faces to "
        "loadCubemap");
  }

  Texture texture;
  texture.internalFormat_ = GL_RGB;  // Cubemaps must be RGB.

  glGenTextures(1, &texture.id_);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture.id_);

  int width, height, numChannels;
  bool initialized = false;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,
                                    &numChannels, /*desired_channels=*/0);
    // Error handling.
    if (data == nullptr) {
      stbi_image_free(data);
      throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + faces[i]);
    }
    if (numChannels != 3) {
      stbi_image_free(data);
      throw TextureException(
          "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
          "Cubemap texture '" +
          faces[i] + "' contained unsupported number of channels: " +
          std::to_string(numChannels));
    }
    if (!initialized) {
      if (width != height) {
        throw TextureException(
            "ERROR::TEXTURE::INVALID_TEXTURE_SIZE\n"
            "Cubemap texture '" +
            faces[i] + "' was not square");
      }
      texture.width_ = width;
      texture.height_ = height;
      texture.numChannels_ = numChannels;
    } else if (width != texture.width_ || height != texture.height_) {
      throw TextureException(
          "ERROR::TEXTURE::INVALID_TEXTURE_SIZE\n"
          "Cubemap texture '" +
          faces[i] + "' was a different size than the first face");
    }

    // Load into the next cube map texture position.
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, /*level=*/0,
                 texture.internalFormat_, width, height, /*border=*/0,
                 /*format=*/GL_RGB, /*type=*/GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }

  // Clamp the 3-dimensional texture lookup so that we don't get seams.
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // No need for mipmapping for a cubemap.
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

Texture Texture::create(int width, int height, GLenum internalFormat) {
  TextureParams params = {.filtering = TextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return create(width, height, internalFormat, params);
}

Texture Texture::create(int width, int height, GLenum internalFormat,
                        const TextureParams& params) {
  Texture texture;
  texture.width_ = width;
  texture.height_ = height;
  texture.numChannels_ = 0;  // Default.
  texture.internalFormat_ = internalFormat;

  glGenTextures(1, &texture.id_);
  glBindTexture(GL_TEXTURE_2D, texture.id_);

  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, texture.internalFormat_,
               texture.width_, texture.height_, 0,
               /* tex data format */ GL_RGBA, GL_FLOAT, /*data=*/nullptr);

  // Set texture-wrapping/filtering options. We disable mipmapping since this
  // is a custom texture.
  // TODO: Theoretically there may be use cases for this?
  if (params.filtering >= TextureFiltering::TRILINEAR) {
    throw TextureException("ERROR::TEXTURE::INVALID_PARAMS");
  }
  applyParams(params);

  return texture;
}

Texture Texture::createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data) {
  TextureParams params = {.filtering = TextureFiltering::BILINEAR,
                          .wrapMode = TextureWrapMode::CLAMP_TO_EDGE};
  return createFromData(width, height, internalFormat, data, params);
}

Texture Texture::createFromData(int width, int height, GLenum internalFormat,
                                const std::vector<glm::vec3>& data,
                                const TextureParams& params) {
  if (data.size() != (width * height)) {
    throw TextureException("ERROR::TEXTURE::INVALID_DATA_SIZE");
  }

  Texture texture = Texture::create(width, height, internalFormat, params);
  // Upload the data.
  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0, texture.internalFormat_,
               texture.width_, texture.height_, 0,
               /* tex data format */ GL_RGB, GL_FLOAT, data.data());
  return texture;
}

void Texture::bindToUnit(unsigned int textureUnit, TextureBindType bindType) {
  // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
  glActiveTexture(GL_TEXTURE0 + textureUnit);

  switch (bindType) {
    case TextureBindType::TEXTURE:
      glBindTexture(GL_TEXTURE_2D, id_);
      break;
    case TextureBindType::CUBEMAP:
      glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
      break;
    case TextureBindType::IMAGE_TEXTURE:
      // Bind image unit.
      glBindImageTexture(textureUnit, id_, /*level=*/0, /*layered=*/GL_FALSE, 0,
                         GL_READ_WRITE, internalFormat_);
      break;
    default:
      throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_BIND_TYPE\n" +
                             std::to_string(static_cast<int>(bindType)));
  }
}

void Texture::applyParams(const TextureParams& params) {
  switch (params.filtering) {
    case TextureFiltering::NEAREST:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;
    case TextureFiltering::BILINEAR:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;
    case TextureFiltering::TRILINEAR:
    case TextureFiltering::ANISOTROPIC:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                      GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      if (params.filtering == TextureFiltering::ANISOTROPIC) {
        constexpr float MAX_ANISOTROPY_SAMPLES = 4.0f;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY,
                        MAX_ANISOTROPY_SAMPLES);
      }
      break;
  }

  switch (params.wrapMode) {
    case TextureWrapMode::REPEAT:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      break;
    case TextureWrapMode::CLAMP_TO_EDGE:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      break;
    case TextureWrapMode::CLAMP_TO_BORDER:
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
                       glm::value_ptr(params.borderColor));
      break;
  }
}

}  // namespace qrk
