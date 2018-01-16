#ifndef QUARKGL_MODEL_H_
#define QUARKGL_MODEL_H_

#include <string>
#include <unordered_map>
#include <vector>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>

#include <qrk/mesh.h>
#include <qrk/shader.h>
#include <qrk/shared.h>
#include <qrk/texture.h>

namespace qrk {
class ModelLoaderException : public QuarkException {
  using QuarkException::QuarkException;
};

constexpr auto DEFAULT_LOAD_FLAGS =
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs;

class Model {
 private:
  unsigned int instanceCount_;
  std::vector<Mesh> meshes_;
  std::string directory_;
  std::unordered_map<std::string, Texture> loadedTextures_;

  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material,
                                            TextureType type);

 public:
  explicit Model(const char* path, unsigned int instanceCount = 0);
  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void draw(Shader shader);
};

}  // namespace qrk

#endif
