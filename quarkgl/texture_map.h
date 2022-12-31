#ifndef QUARKGL_TEXTURE_MAP_H_
#define QUARKGL_TEXTURE_MAP_H_

#include <assimp/scene.h>
#include <qrk/exceptions.h>
#include <qrk/texture.h>

#include <string>
#include <vector>

namespace qrk {

class TextureMapException : public QuarkException {
  using QuarkException::QuarkException;
};

// The type of map, i.e. how the underlying texture is meant to be used.
enum class TextureMapType {
  DIFFUSE = 0,
  SPECULAR,
  ROUGHNESS,
  METALLIC,
  AO,
  EMISSION,
  NORMAL,
  CUBEMAP,
  // Adding a new texture type?
  // Update allTextureTypes() below.
};

inline std::vector<TextureMapType> allTextureTypes() {
  std::vector<TextureMapType> textureTypes;
  for (int i = 0; i <= static_cast<int>(TextureMapType::CUBEMAP); i++) {
    textureTypes.push_back(static_cast<TextureMapType>(i));
  }
  return textureTypes;
}

inline std::vector<aiTextureType> textureMapTypeToAiTextureTypes(
    TextureMapType type) {
  switch (type) {
    case TextureMapType::DIFFUSE:
      return {aiTextureType_DIFFUSE};
    case TextureMapType::SPECULAR:
      // Use metalness for specularity as well. When this is loaded as a
      // combined "metalnessRoughnessTexture", shaders should read the blue
      // channel.
      return {aiTextureType_SPECULAR, aiTextureType_METALNESS};
    case TextureMapType::ROUGHNESS:
      return {aiTextureType_DIFFUSE_ROUGHNESS};
    case TextureMapType::METALLIC:
      return {aiTextureType_METALNESS};
    case TextureMapType::AO:
      return {aiTextureType_AMBIENT_OCCLUSION};
    case TextureMapType::EMISSION:
      return {aiTextureType_EMISSIVE};
    case TextureMapType::NORMAL:
      return {aiTextureType_NORMALS};
    default:
      throw TextureMapException(
          "ERROR::TEXTURE_MAP::INVALID_TEXTURE_MAP_TYPE\n" +
          std::to_string(static_cast<int>(type)));
  }
}

// A thin wrapper around a texture, with special properties.
class TextureMap {
 public:
  TextureMap(const Texture& texture, TextureMapType type)
      : texture_(texture), type_(type) {}

  Texture& getTexture() { return texture_; }
  TextureMapType getType() const { return type_; }

 private:
  Texture texture_;
  TextureMapType type_;
};
}  // namespace qrk

#endif
