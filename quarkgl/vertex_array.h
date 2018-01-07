#ifndef QUARKGL_VERTEX_ARRAY_H_
#define QUARKGL_VERTEX_ARRAY_H_

#include <vector>

#include <glad/glad.h>

namespace qrk {

class VertexArray {
 private:
  struct VertexAttrib {
    unsigned int layoutPosition;
    unsigned int size;
    unsigned int type;
  };

  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int ebo_ = 0;

  unsigned int vertexSize_ = 0;
  unsigned int elementSize_ = 0;

  std::vector<VertexAttrib> attribs_;
  unsigned int stride_ = 0;

 public:
  VertexArray();
  // TODO: Can we have a destructor here?
  unsigned int getVao() { return vao_; }
  unsigned int getVbo() { return vbo_; }
  unsigned int getEbo() { return ebo_; }

  void activate();
  void deactivate();
  void loadVertexData(const std::vector<char>& data);
  void loadVertexData(const void* data, unsigned int size);
  void loadElementData(const std::vector<unsigned int>& indices);
  void loadElementData(const unsigned int* indices, unsigned int size);
  void addVertexAttrib(unsigned int size, unsigned int type);
  void finalizeVertexAttribs();
};
}  // namespace qrk

#endif
