#include <qrk/vertex_array.h>

namespace qrk {

VertexArray::VertexArray() {
  glGenVertexArrays(1, &vao_);
  activate();
}

void VertexArray::activate() { glBindVertexArray(vao_); }

void VertexArray::deactivate() { glBindVertexArray(0); }

// TODO: Reduce duplication in these methods.
void VertexArray::loadVertexData(const std::vector<char>& data) {
  activate();

  if (!vbo_) glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  // TODO: Allow other draw strategies besides GL_STATIC_DRAW.
  glBufferData(GL_ARRAY_BUFFER, data.size(), &data[0], GL_STATIC_DRAW);
  vertexSize_ = data.size();
}

void VertexArray::loadVertexData(const void* data, unsigned int size) {
  activate();

  if (!vbo_) glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  vertexSize_ = size;
}

void VertexArray::allocateInstanceVertexData(unsigned int size) {
  activate();

  if (!instanceVbo_) glGenBuffers(1, &instanceVbo_);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
  glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
}

void VertexArray::loadInstanceVertexData(const std::vector<char>& data) {
  activate();

  if (!instanceVbo_) glGenBuffers(1, &instanceVbo_);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
  glBufferData(GL_ARRAY_BUFFER, data.size(), &data[0], GL_STATIC_DRAW);
}

void VertexArray::loadInstanceVertexData(const void* data, unsigned int size) {
  activate();

  if (!instanceVbo_) glGenBuffers(1, &instanceVbo_);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVbo_);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexArray::loadElementData(const std::vector<unsigned int>& indices) {
  activate();

  if (!ebo_) glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size(), &indices[0],
               GL_STATIC_DRAW);
  elementSize_ = indices.size();
}

void VertexArray::loadElementData(const unsigned int* indices,
                                  unsigned int size) {
  activate();

  if (!ebo_) glGenBuffers(1, &ebo_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  elementSize_ = size;
}

void VertexArray::addVertexAttrib(unsigned int size, unsigned int type,
                                  unsigned int instanceDivisor) {
  VertexAttrib attrib = {
      .layoutPosition = nextLayoutPosition_,
      .size = size,
      .type = type,
      .instanceDivisor = instanceDivisor,
  };
  attribs_.push_back(attrib);
  nextLayoutPosition_++;
  // TODO: Support types other than float.
  stride_ += size * sizeof(float);
}

void VertexArray::finalizeVertexAttribs() {
  activate();

  int offset = 0;
  for (const VertexAttrib& attrib : attribs_) {
    glVertexAttribPointer(
        attrib.layoutPosition, attrib.size, attrib.type,
        /* normalized */ GL_FALSE, stride_,
        // TODO: Support types other than float.
        /* offset */ static_cast<const char*>(nullptr) + offset);
    glEnableVertexAttribArray(attrib.layoutPosition);
    if (attrib.instanceDivisor) {
      glVertexAttribDivisor(attrib.layoutPosition, attrib.instanceDivisor);
    }
    offset += attrib.size * sizeof(float);
  }

  // Clear state to support subsequent runs.
  // We intentionally don't reset nextLayoutPosition_.
  attribs_.clear();
  stride_ = 0;
}
}  // namespace qrk
