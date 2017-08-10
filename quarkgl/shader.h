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
  Shader(const char* vertexPath, const char* fragmentPath);
  unsigned int getProgramId() { return shaderProgram_; }

  void use();
  void setBool(const char* name, bool value);
  void setInt(const char* name, int value);
  void setFloat(const char* name, float value);
  void setVec3(const char* name, glm::vec3& vector);
  void setVec3(const char* name, float v0, float v1, float v2);
  void setMat4(const char* name, glm::mat4& matrix);
};
}

#endif
