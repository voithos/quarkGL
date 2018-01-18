#include <glad/glad.h>
#include <stb/stb_image.h>

#include <qrk/texture.h>

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_image_free(data);

  return texture;
}

}  // namespace qrk
