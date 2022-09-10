#ifndef QUARKGL_VERTEX_ARRAY_H_
#define QUARKGL_VERTEX_ARRAY_H_

#include <glad/glad.h>

#include <vector>

namespace qrk {

class VertexArray {
 public:
  VertexArray();
  // TODO: Can we have a destructor here?
  unsigned int getVao() { return vao_; }
  unsigned int getVbo() { return vbo_; }
  unsigned int getInstanceVbo() { return instanceVbo_; }
  unsigned int getEbo() { return ebo_; }

  void activate();
  void deactivate();
  void loadVertexData(const std::vector<char>& data);
  void loadVertexData(const void* data, unsigned int size);
  void allocateInstanceVertexData(unsigned int size);
  void loadInstanceVertexData(const std::vector<char>& data);
  void loadInstanceVertexData(const void* data, unsigned int size);
  void loadElementData(const std::vector<unsigned int>& indices);
  void loadElementData(const unsigned int* indices, unsigned int size);
  void addVertexAttrib(unsigned int size, unsigned int type,
                       unsigned int instanceDivisor = 0);
  void finalizeVertexAttribs();

 private:
  struct VertexAttrib {
    unsigned int layoutPosition;
    unsigned int size;
    unsigned int type;
    unsigned int instanceDivisor;
  };

  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int instanceVbo_ = 0;
  unsigned int ebo_ = 0;

  unsigned int vertexSizeBytes_ = 0;
  unsigned int elementSize_ = 0;

  std::vector<VertexAttrib> attribs_;
  unsigned int nextLayoutPosition_ = 0;
  unsigned int stride_ = 0;
};
}  // namespace qrk

#endif
