#include <glad/glad.h>
#include <qrk/texture.h>
#include <stb/stb_image.h>

namespace qrk {

unsigned int loadTexture(const char* path, bool isSRGB) {
  int width, height, numChannels;
  unsigned char* data = stbi_load(path, &width, &height, &numChannels, 0);

  if (data == nullptr) {
    stbi_image_free(data);
    throw TextureException("ERROR::TEXTURE::LOAD_FAILED\n" + std::string(path));
  }

  GLenum internalFormat;
  GLenum dataFormat;
  if (numChannels == 1) {
    internalFormat = GL_RED;
    dataFormat = GL_RED;
  } else if (numChannels == 3) {
    internalFormat = isSRGB ? GL_SRGB : GL_RGB;
    dataFormat = GL_RGB;
  } else if (numChannels == 4) {
    internalFormat = isSRGB ? GL_SRGB_ALPHA : GL_RGBA;
    dataFormat = GL_RGBA;
  } else {
    stbi_image_free(data);
    throw TextureException(
        "ERROR::TEXTURE::UNSUPPORTED_TEXTURE_FORMAT\n"
        "Texture '" +
        std::string(path) + "' contained unsupported number of channels: " +
        std::to_string(numChannels));
  }

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D, /* mipmap level */ 0,
               /* texture format */ internalFormat, width, height, 0,
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

unsigned int loadCubemap(std::vector<std::string> faces) {
  if (faces.size() != 6) {
    throw TextureException(
        "ERROR::TEXTURE::INVALID_ARGUMENT\nMust pass exactly 6 faces to "
        "loadCubemap");
  }

  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

  int width, height, numChannels;
  for (unsigned int i = 0; i < faces.size(); i++) {
    unsigned char* data = stbi_load(faces[i].c_str(), &width, &height,
                                    &numChannels, /*dired_channels=*/0);
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
    // Load into the next cube map texture position.
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, /*level=*/0,
                 /*internalformat=*/GL_RGB, width, height, /*border=*/0,
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

}  // namespace qrk
