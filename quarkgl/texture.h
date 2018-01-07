#ifndef QUARKGL_TEXTURE_H_
#define QUARKGL_TEXTURE_H_

#include <string>
#include <vector>

#include <assimp/scene.h>

#include <qrk/shared.h>

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
unsigned int loadTexture(const char* path);

struct Texture {
  unsigned int id;
  TextureType type;
  std::string path;
};
}  // namespace qrk

#endif
