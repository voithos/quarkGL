#ifndef QUARKGL_SHADER_LOADER_H_
#define QUARKGL_SHADER_LOADER_H_

#include <qrk/exceptions.h>
#include <qrk/shader_defs.h>

#include <deque>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace qrk {

class ShaderLoaderException : public QuarkException {
  using QuarkException::QuarkException;
};

class ShaderLoader {
 public:
  ShaderLoader(const ShaderSource* shaderSource, const ShaderType type);
  std::string load();

 private:
  void checkShaderType(std::string const& shaderPath);
  bool alreadyLoadedOnce(std::string const& shaderPath);
  void cacheShaderCode(std::string const& shaderPath,
                       std::string const& shaderCode);
  std::string load(std::string const& shaderPath);
  std::string lookupOrLoad(std::string const& shaderPath);
  std::string preprocessShader(std::string const& shaderPath,
                               std::string const& shaderCode);
  std::string getIncludesTraceback();
  bool checkCircularInclude(std::string const& resolvedPath);

  const ShaderSource* shaderSource_;
  const ShaderType shaderType_;
  std::deque<std::string> includeChain_;
  std::unordered_map<std::string, std::string> codeCache_;
  std::unordered_set<std::string> onceCache_;
};
}  // namespace qrk

#endif
