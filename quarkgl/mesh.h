#ifndef QUARKGL_MESH_H_
#define QUARKGL_MESH_H_

#include <glad/glad.h>
#include <qrk/shader.h>
#include <qrk/texture_map.h>
#include <qrk/texture_registry.h>
#include <qrk/vertex_array.h>

#include <glm/glm.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace qrk {

class Renderable {
 public:
  virtual ~Renderable() = default;

  glm::mat4 getModelTransform() const { return model_; }
  void setModelTransform(const glm::mat4& model) { model_ = model; }

  // TODO: Add translation, rotation, scale methods.

  virtual void draw(Shader& shader,
                    TextureRegistry* textureRegistry = nullptr) {
    glm::mat4 transform(1.0f);
    return drawWithTransform(transform, shader, textureRegistry);
  }
  virtual void drawWithTransform(
      const glm::mat4& transform, Shader& shader,
      TextureRegistry* textureRegistry = nullptr) = 0;

 protected:
  // The model transform matrix.
  glm::mat4 model_ = glm::mat4(1.0f);
};

// A node in a tree of Renderables, containing one or more renderables.
// When rendering, each node's transform is applied to the transform of its
// Renderables, as well as to its child RenderableNodes.
class RenderableNode : public Renderable {
 public:
  virtual ~RenderableNode() = default;
  void drawWithTransform(const glm::mat4& transform, Shader& shader,
                         TextureRegistry* textureRegistry = nullptr) override;

  void addRenderable(std::unique_ptr<Renderable> renderable) {
    renderables_.push_back(std::move(renderable));
  }

  void addChildNode(std::unique_ptr<RenderableNode> childNode) {
    childNodes_.push_back(std::move(childNode));
  }

  void visitRenderables(std::function<void(Renderable*)> visitor);

 protected:
  // The set of Renderables making up this node.
  std::vector<std::unique_ptr<Renderable>> renderables_;
  // The set of child RenderableNodes.
  std::vector<std::unique_ptr<RenderableNode>> childNodes_;
};

// An abstract class that represents a triangle mesh and handles loading and
// rendering. Child classes can specialize when configuring vertex attributes.
class Mesh : public Renderable {
 public:
  virtual ~Mesh() = default;

  void loadInstanceModels(const std::vector<glm::mat4>& models);
  void loadInstanceModels(const glm::mat4* models, unsigned int size);
  void drawWithTransform(const glm::mat4& transform, Shader& shader,
                         TextureRegistry* textureRegistry = nullptr) override;

  std::vector<unsigned int> getIndices() { return indices_; }
  std::vector<TextureMap> getTextureMaps() { return textureMaps_; }

 protected:
  // Loads mesh data into the mesh. Calls initializeVertexAttributes and
  // initializeVertexArrayInstanceData under the hood. Must be called
  // immediately after construction.
  virtual void loadMeshData(const void* vertexData, unsigned int numVertices,
                            unsigned int vertexSizeBytes,
                            const std::vector<unsigned int>& indices,
                            const std::vector<TextureMap>& textureMaps,
                            unsigned int instanceCount = 0);
  // Initializes vertex attributes.
  virtual void initializeVertexAttributes() = 0;
  // Allocates and initializes vertex array instance data.
  virtual void initializeVertexArrayInstanceData();
  // Binds texture maps to texture units and sets shader sampler uniforms.
  virtual void bindTextures(Shader& shader, TextureRegistry* textureRegistry);
  // Emits glDraw* calls based on the mesh instancing/indexing. Requires shaders
  // and VAOs to be active prior to calling.
  virtual void glDraw();

  VertexArray vertexArray_;
  std::vector<unsigned int> indices_;
  std::vector<TextureMap> textureMaps_;

  // The number of vertices in the mesh.
  unsigned int numVertices_;
  // The size, in bytes, of each vertex.
  unsigned int vertexSizeBytes_;
  unsigned int instanceCount_;
};

}  // namespace qrk

#endif
