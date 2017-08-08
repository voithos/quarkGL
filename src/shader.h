#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace qrk {

class Shader {
private:
  unsigned int shaderProgram_;

public:
  Shader(const char *vertexPath, const char *fragmentPath);
  unsigned int getProgramId() { return shaderProgram_; }

  void use();
  void setBool(const char *name, bool value) const;
  void setInt(const char *name, int value) const;
  void setFloat(const char *name, float value) const;
  void setMatrix4f(const char *name, glm::mat4 &matrix) const;
};
}

#endif
