#ifndef QUARKGL_MODEL_H_
#define QUARKGL_MODEL_H_

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <qrk/mesh.h>
#include <qrk/shader.h>
#include <qrk/shared.h>

namespace qrk {
class ModelLoaderException : public QuarkException {
  using QuarkException::QuarkException;
};

const auto DEFAULT_LOAD_FLAGS =
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs;

inline const aiTextureType textureTypeToAiTextureType(TextureType type) {
  switch (type) {
    case TextureType::DIFFUSE:
      return aiTextureType_DIFFUSE;
    case TextureType::SPECULAR:
      return aiTextureType_SPECULAR;
    case TextureType::EMISSION:
      return aiTextureType_EMISSIVE;
  }
}

class Model {
 private:
  std::vector<Mesh> meshes_;
  std::string directory_;
  std::unordered_map<std::string, Texture> loadedTextures_;

  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material,
                                            TextureType type);

 public:
  Model(const char* path);
  void draw(Shader shader);
};

}  // namespace qrk

#endif
