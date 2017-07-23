#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
private:
  unsigned int shaderProgram;

public:
  Shader(const char *vertexPath, const char *fragmentPath);
  unsigned int getProgramId();
  void use();
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setMatrix4f(const std::string &name, glm::mat4 &matrix) const;
};

#endif
