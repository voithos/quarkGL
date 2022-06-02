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

// An abstract class that represents a triangle mesh and handles loading and
// rendering. Child classes can specialize when configuring vertex attributes.
class Mesh {
 public:
  virtual ~Mesh() = default;

  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void draw(Shader& shader);

  std::vector<unsigned int> getIndices() { return indices_; }
  std::vector<Texture> getTextures() { return textures_; }

 protected:
  // Loads mesh data into the mesh. Calls initializeVertexAttributes and
  // initializeVertexArrayInstanceData under the hood. Must be called
  // immediately after construction.
  virtual void loadMeshData(const void* vertexData, unsigned int numVertices,
                            unsigned int vertexSize,
                            std::vector<unsigned int> indices,
                            std::vector<Texture> textures,
                            unsigned int instanceCount = 0);
  // Initializes vertex attributes.
  virtual void initializeVertexAttributes() = 0;
  // Allocates and initializes vertex array instance data.
  virtual void initializeVertexArrayInstanceData();
  // Binds textures to texture units and sets shader sampler uniforms.
  virtual void bindTextures(Shader& shader);
  // Emits glDraw* calls based on the mesh instancing/indexing. Requires shaders
  // and VAOs to be active prior to calling.
  virtual void glDraw();

  VertexArray vertexArray_;
  std::vector<unsigned int> indices_;
  std::vector<Texture> textures_;

  // The number of vertices in the mesh.
  unsigned int numVertices_;
  // The size, in bytes, of each vertex.
  unsigned int vertexSize_;
  unsigned int instanceCount_;
};

}  // namespace qrk

#endif
