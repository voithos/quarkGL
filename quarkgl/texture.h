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
  DIFFUSE = 0,
  SPECULAR,
  EMISSION,
  CUBEMAP,
  // Adding a new texture type?
  // Update allTextureTypes() below.
};

inline std::vector<TextureType> allTextureTypes() {
  std::vector<TextureType> textureTypes;
  for (int i = 0; i <= static_cast<int>(TextureType::CUBEMAP); i++) {
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
    default:
      throw TextureException("ERROR::TEXTURE::INVALID_TEXTURE_TYPE\n" +
                             std::to_string(static_cast<int>(type)));
  }
}

class Texture {
 public:
  // Loads a texture from a given path.
  // TODO: Consider putting this in a TextureLoader class.
  static Texture load(const char* path, TextureType type, bool isSRGB = true);

  // Loads a cubemap from a set of 6 textures for the faces. Textures must be
  // passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  static Texture loadCubemap(std::vector<std::string> faces);

  unsigned int getId() const { return id_; }
  TextureType getType() const { return type_; }
  // Returns the path to a texture. Not applicable for cubemaps or generated
  // textures.
  std::string getPath() const { return path_; }
  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  int getNumChannels() const { return numChannels_; }

 private:
  unsigned int id_;
  TextureType type_;
  std::string path_;
  int width_;
  int height_;
  int numChannels_;
};
}  // namespace qrk

#endif
