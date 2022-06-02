#ifndef QUARKGL_MESH_PRIMITIVES_H_
#define QUARKGL_MESH_PRIMITIVES_H_

#include <qrk/mesh.h>

namespace qrk {

class CubeMesh : public Mesh {
 public:
  CubeMesh();

 protected:
  void initializeVertexAttributes() override;
};

}  // namespace qrk

#endif
