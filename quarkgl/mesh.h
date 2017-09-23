#ifndef QUARKGL_MESH_H_
#define QUARKGL_MESH_H_

#include <sstream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <qrk/shader.h>
#include <qrk/vertex_array.h>

namespace qrk {
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoords;
};

enum class TextureType {
  DIFFUSE,
  SPECULAR,
  EMISSION,
};

struct Texture {
  unsigned int id;
  TextureType type;
};

class Mesh {
 private:
  VertexArray vertexArray_;
  std::vector<Vertex> vertices_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;

 public:
  Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices,
       std::vector<Texture> textures);

  void draw(Shader shader);

  std::vector<Vertex> getVertices() { return vertices_; }
  std::vector<unsigned int> getIndices() { return indices_; }
  std::vector<Texture> getTextures() { return textures_; }
};
}

#endif
