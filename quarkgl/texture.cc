#include <glad/glad.h>
#include <qrk/texture.h>
#include <stb/stb_image.h>

namespace qrk {

Texture Texture::load(const char* path, TextureType type, bool isSRGB) {
  Texture texture;
  texture.type_ = type;

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
  // TODO: Make some of these configurable.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4.0f);

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
  texture.type_ = TextureType::CUBEMAP;
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
  Texture texture;
  texture.type_ = TextureType::CUSTOM;
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
  // TODO: Make some of these configurable.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

void Texture::bindToUnit(unsigned int textureUnit) {
  // TODO: Take into account GL_MAX_TEXTURE_UNITS here.
  glActiveTexture(GL_TEXTURE0 + textureUnit);

  if (type_ == TextureType::CUBEMAP) {
    glBindTexture(GL_TEXTURE_CUBE_MAP, id_);
  } else if (type_ == TextureType::CUSTOM) {
    // Bind image unit.
    glBindImageTexture(textureUnit, id_, /*level=*/0, /*layered=*/GL_FALSE, 0,
                       GL_READ_WRITE, internalFormat_);
  } else {
    glBindTexture(GL_TEXTURE_2D, id_);
  }
}

}  // namespace qrk
