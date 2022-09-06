#ifndef QUARKGL_MODEL_H_
#define QUARKGL_MODEL_H_

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <qrk/exceptions.h>
#include <qrk/mesh.h>
#include <qrk/shader.h>
#include <qrk/texture_map.h>

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
  glm::vec3 tangent;
  glm::vec2 texCoords;
};

class ModelMesh : public Mesh {
 public:
  ModelMesh(const std::vector<ModelVertex>& vertices,
            const std::vector<unsigned int>& indices,
            const std::vector<TextureMap>& textureMaps,
            unsigned int instanceCount = 0);

  virtual ~ModelMesh() = default;

 private:
  void initializeVertexAttributes() override;
  std::vector<ModelVertex> vertices_;
};

constexpr auto DEFAULT_LOAD_FLAGS =
    // Ensure that all non-triangular polygon are converted to triangles.
    aiProcess_Triangulate |
    // Generate normals if the model doesn't have them.
    aiProcess_GenNormals |
    // Calculates tangent space if the model doesn't have them.
    aiProcess_CalcTangentSpace |
    // Generate UV coords if they aren't present.
    aiProcess_GenUVCoords |
    // Reduce vertex duplication.
    aiProcess_JoinIdenticalVertices |
    // Sort the result by primitive type.
    aiProcess_SortByPType;

class Model : public Renderable {
 public:
  explicit Model(const char* path, unsigned int instanceCount = 0);
  virtual ~Model() = default;
  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void draw(Shader& shader,
            TextureRegistry* textureRegistry = nullptr) override;

 private:
  void loadModel(std::string path);
  void processNode(aiNode* node, const aiScene* scene);
  ModelMesh processMesh(aiMesh* mesh, const aiScene* scene);
  std::vector<TextureMap> loadMaterialTextureMaps(aiMaterial* material,
                                                  TextureMapType type);

  unsigned int instanceCount_;
  std::vector<ModelMesh> meshes_;
  std::string directory_;
  std::unordered_map<std::string, TextureMap> loadedTextureMaps_;
};

}  // namespace qrk

#endif
