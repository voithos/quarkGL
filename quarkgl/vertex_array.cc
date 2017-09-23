#include <qrk/vertex_array.h>

namespace qrk {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &vao_);
  activate();
}

void VertexArray::activate() { glBindVertexArray(vao_); }

void VertexArray::deactivate() { glBindVertexArray(0); }

void VertexArray::loadVertexData(const std::vector<char>& data) {
  activate();

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, data.size(), &data[0], GL_STATIC_DRAW);
  vertexSize_ = data.size();
}

void VertexArray::loadVertexData(const void* data, unsigned int size) {
  activate();

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  vertexSize_ = size;
}

void VertexArray::loadElementData(const std::vector<unsigned int>& indices) {
  activate();

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), &indices[0],
               GL_STATIC_DRAW);
  elementSize_ = indices.size();
}

void VertexArray::loadElementData(const unsigned int* indices,
                                  unsigned int size) {
  activate();

  glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  elementSize_ = size;
}

void VertexArray::addVertexAttrib(unsigned int size, unsigned int type) {
  VertexAttrib attrib = {
      .layoutPosition = static_cast<unsigned int>(attribs_.size()),
      .size = size,
      .type = type,
  };
  attribs_.push_back(attrib);
  // TODO: Support types other than float.
  stride_ += size * sizeof(float);
}

void VertexArray::finalizeVertexAttribs() {
  activate();

  int offset = 0;
  for (const VertexAttrib& attrib : attribs_) {
    glVertexAttribPointer(attrib.layoutPosition, attrib.size, attrib.type,
                          /* normalized */ GL_FALSE, stride_,
                          // TODO: Support types other than float.
                          /* offset */ static_cast<const char*>(0) + offset);
    glEnableVertexAttribArray(attrib.layoutPosition);
    offset += attrib.size * sizeof(float);
  }
}
}
