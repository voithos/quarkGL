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
  CubeMesh();

 protected:
  void initializeVertexAttributes() override;
};

}  // namespace qrk

#endif
