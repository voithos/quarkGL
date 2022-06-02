#ifndef QUARKGL_TEXTURE_H_
#define QUARKGL_TEXTURE_H_

#include <assimp/scene.h>
#include <qrk/exceptions.h>

#include <string>
#include <vector>

namespace qrk {

class TextureException : public QuarkException {
  using QuarkException::QuarkException;
};

enum class TextureType {
  DIFFUSE,
  SPECULAR,
  EMISSION,
  // Adding a new texture type?
  // Update allTextureTypes() below.
};

inline std::vector<TextureType> allTextureTypes() {
  std::vector<TextureType> textureTypes;
  for (int i = 0; i <= static_cast<int>(TextureType::EMISSION); i++) {
    textureTypes.push_back(static_cast<TextureType>(i));
  }
  return textureTypes;
}

inline const aiTextureType textureTypeToAiTextureType(TextureType type) {
  switch (type) {
    case TextureType::DIFFUSE:
      return aiTextureType_DIFFUSE;
    case TextureType::SPECULAR:
      return aiTextureType_SPECULAR;
    case TextureType::EMISSION:
      return aiTextureType_EMISSIVE;
  }
  throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" +
                         std::to_string(static_cast<int>(type)));
}

// TODO: Consider putting this in a TextureLoader class.
unsigned int loadTexture(const char* path, bool isSRGB = true);

// Loads a cubemap from a set of 6 textures for the faces. Textures must be
// passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and incrementing
// from there; namely, in the order right, left, top, bottom, front, and back.
unsigned int loadCubemap(std::vector<std::string> faces);

struct Texture {
  unsigned int id;
  TextureType type;
  std::string path;
};
}  // namespace qrk

#endif
