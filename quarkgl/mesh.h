#ifndef QUARKGL_MESH_H_
#define QUARKGL_MESH_H_

#include <glad/glad.h>
#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/vertex_array.h>

#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace qrk {
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

class Mesh {
 public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures, unsigned int instanceCount = 0);

  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void draw(Shader shader);

  std::vector<Vertex> getVertices() { return vertices_; }
  std::vector<unsigned int> getIndices() { return indices_; }
  std::vector<Texture> getTextures() { return textures_; }

 private:
  VertexArray vertexArray_;
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;
  unsigned int instanceCount_;
};
}  // namespace qrk

#endif
