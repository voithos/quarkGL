#ifndef QUARKGL_SHADER_DEFS_H_
#define QUARKGL_SHADER_DEFS_H_

namespace qrk {

enum class ShaderType {
  VERTEX,
  FRAGMENT,
  GEOMETRY,
  COMPUTE,
};

struct ShaderSource {
  inline explicit ShaderSource(const char* value) : value(value) {}
  const char* value;
  virtual bool isPath() const = 0;
};

struct ShaderInline : ShaderSource {
  using ShaderSource::ShaderSource;
  bool isPath() const { return false; }
};

struct ShaderPath : ShaderSource {
  using ShaderSource::ShaderSource;
  bool isPath() const { return true; }
};

}  // namespace qrk

#endif
