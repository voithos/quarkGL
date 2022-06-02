#ifndef QUARKGL_MODEL_H_
#define QUARKGL_MODEL_H_

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <qrk/exceptions.h>
#include <qrk/mesh.h>
#include <qrk/shader.h>
#include <qrk/texture.h>

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace qrk {
class ModelLoaderException : public QuarkException {
  using QuarkException::QuarkException;
};

struct ModelVertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

class ModelMesh : public Mesh {
 public:
  ModelMesh(std::vector<ModelVertex> vertices,
            std::vector<unsigned int> indices, std::vector<Texture> textures,
            unsigned int instanceCount = 0);

 private:
  void initializeVertexAttributes() override;
  std::vector<ModelVertex> vertices_;
};

constexpr auto DEFAULT_LOAD_FLAGS =
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs;

class Model : public Renderable {
 public:
  explicit Model(const char* path, unsigned int instanceCount = 0);
  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void draw(Shader& shader) override;

 private:
  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  ModelMesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<Texture> loadMaterialTextures(aiMaterial* material,
                                            TextureType type);

  unsigned int instanceCount_;
  std::vector<ModelMesh> meshes_;
  std::string directory_;
  std::unordered_map<std::string, Texture> loadedTextures_;
};

}  // namespace qrk

#endif
