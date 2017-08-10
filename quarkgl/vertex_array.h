#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

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

  unsigned int vao_;
  unsigned int vbo_;
  unsigned int ebo_;

  unsigned int vertexSize_ = 0;
  unsigned int elementSize_ = 0;

  std::vector<VertexAttrib> attribs_;
  unsigned int stride_ = 0;

 public:
  VertexArray();
  unsigned int getVao() { return vao_; }
  unsigned int getVbo() { return vbo_; }
  unsigned int getEbo() { return ebo_; }

  void use();
  void loadVertexData(const std::vector<char>& data);
  void loadVertexData(const void* data, unsigned int size);
  void loadElementData(const std::vector<unsigned int>& indices);
  void loadElementData(const unsigned int* indices, unsigned int size);
  void addVertexAttrib(unsigned int size, unsigned int type);
  void finalizeVertexAttribs();
};
}

#endif
