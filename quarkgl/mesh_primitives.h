#ifndef QUARKGL_MESH_PRIMITIVES_H_
#define QUARKGL_MESH_PRIMITIVES_H_

#include <qrk/mesh.h>

namespace qrk {

class PrimitiveMesh : public Mesh {
 public:
  virtual void draw(Shader& shader) override;
};

class CubeMesh : public PrimitiveMesh {
 public:
  CubeMesh(std::string texturePath = "");

 protected:
  void initializeVertexAttributes() override;
};

class SkyboxMesh : public PrimitiveMesh {
 public:
  // Creates a new skybox mesh from a set of 6 textures for the faces. Textures
  // must be passed in order starting with GL_TEXTURE_CUBE_MAP_POSITIVE_X and
  // incrementing from there; namely, in the order right, left, top, bottom,
  // front, and back.
  SkyboxMesh(std::vector<std::string> faces);

 protected:
  void initializeVertexAttributes() override;
};

}  // namespace qrk

#endif
