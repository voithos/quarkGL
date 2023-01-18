#include <qrk/exceptions.h>
#include <qrk/shader_defs.h>
#include <qrk/shader_loader.h>
#include <qrk/utils.h>

#include <fstream>
#include <regex>
#include <sstream>

namespace qrk {

std::string readFile(std::string const& path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  file.open(path.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void ShaderLoader::checkShaderType(std::string const& shaderPath) {
  // Allow ".glsl" as a generic shader suffix (e.g. for type-agnostic shader
  // code).
  if (string_has_suffix(shaderPath, ".glsl")) return;

  switch (shaderType_) {
    case ShaderType::VERTEX:
      if (!string_has_suffix(shaderPath, ".vert")) {
        throw ShaderLoaderException(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded vertex shader '" +
            shaderPath + "' must end in '.vert'.");
      }
      break;
    case ShaderType::FRAGMENT:
      if (!string_has_suffix(shaderPath, ".frag")) {
        throw ShaderLoaderException(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded fragment shader '" +
            shaderPath + "' must end in '.frag'.");
      }
      break;
    case ShaderType::GEOMETRY:
      if (!string_has_suffix(shaderPath, ".geom")) {
        throw ShaderLoaderException(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded geometry shader '" +
            shaderPath + "' must end in '.geom'.");
      }
      break;
    case ShaderType::COMPUTE:
      if (!string_has_suffix(shaderPath, ".comp")) {
        throw ShaderLoaderException(
            "ERROR::SHADER_LOADER::INVALID_EXTENSION\n"
            "Loaded compute shader '" +
            shaderPath + "' must end in '.comp'.");
      }
      break;
  }
}

bool ShaderLoader::alreadyLoadedOnce(std::string const& shaderPath) {
  std::string resolvedPath = resolvePath(shaderPath);
  auto item = onceCache_.find(resolvedPath);
  return item != onceCache_.end();
}

std::string ShaderLoader::getIncludesTraceback() {
  std::stringstream buffer;
  for (std::string path : includeChain_) {
    buffer << "  > " << path << std::endl;
  }
  return buffer.str();
}

bool ShaderLoader::checkCircularInclude(std::string const& resolvedPath) {
  for (std::string path : includeChain_) {
    if (path == resolvedPath) {
      return true;
    }
  }
  return false;
}

ShaderLoader::ShaderLoader(const ShaderSource* shaderSource,
                           const ShaderType type)
    : shaderSource_(shaderSource), shaderType_(type) {}

std::string ShaderLoader::lookupOrLoad(std::string const& shaderPath) {
  std::string resolvedPath = resolvePath(shaderPath);

  auto item = codeCache_.find(resolvedPath);
  if (item != codeCache_.end()) {
    // Cache hit; return code.
    return item->second;
  }

  // Cache miss; read code from file.
  std::string shaderCode;
  try {
    shaderCode = readFile(shaderPath);
  } catch (std::ifstream::failure& e) {
    std::string traceback = getIncludesTraceback();
    throw ShaderLoaderException(
        "ERROR::SHADER_LOADER::FILE_NOT_SUCCESSFULLY_READ\n"
        "Unable to read shader '" +
        std::string(shaderPath) + "', traceback below (most recent last):\n" +
        traceback);
  }
  // Don't cache the code just yet, because we need to preprocess it.
  return shaderCode;
}

std::string ShaderLoader::load(std::string const& shaderPath) {
  checkShaderType(shaderPath);

  // Handle #pragma once.
  if (alreadyLoadedOnce(shaderPath)) {
    return "";
  }

  auto shaderCode = lookupOrLoad(shaderPath);
  auto processedCode = preprocessShader(shaderPath, shaderCode);
  cacheShaderCode(shaderPath, processedCode);
  return processedCode;
}

void ShaderLoader::cacheShaderCode(std::string const& shaderPath,
                                   std::string const& shaderCode) {
  std::string resolvedPath = resolvePath(shaderPath);
  codeCache_[resolvedPath] = shaderCode;
}

std::string ShaderLoader::preprocessShader(std::string const& shaderPath,
                                           std::string const& shaderCode) {
  std::string resolvedPath = resolvePath(shaderPath);
  includeChain_.push_back(resolvedPath);

  std::regex oncePattern(R"(((^|\r?\n)\s*)#pragma\s+once\s*(?=\r?\n|$))");
  if (std::regex_search(shaderCode, oncePattern)) {
    onceCache_.insert(resolvedPath);
  }

  std::regex includePattern(
      R"(((^|\r?\n)\s*)#pragma\s+qrk_include\s+(".*"|<.*>)(?=\r?\n|$))");
  std::string processedCode = regexReplace(
      shaderCode, includePattern, [this, shaderPath](const std::smatch& m) {
        std::string whitespace = m[1];
        // Extract the path.
        std::string incl = m[3];
        char inclType = incl[0];
        std::string path = trim(incl.substr(1, incl.size() - 2));

        if (inclType == '<') {
          // qrk include.
          return whitespace + load("quarkgl/shaders/" + path);
        } else {
          // Standard include.
          size_t i = shaderPath.find_last_of("/");
          // This will either be the current shader's directory, or empty
          // string if the current shader is at project root.
          std::string prefix =
              i != std::string::npos ? shaderPath.substr(0, i + 1) : "";
          return whitespace + load(prefix + path);
        }
      });

  includeChain_.pop_back();
  return processedCode;
}

std::string ShaderLoader::load() {
  // Handle either loading from file, or loading from inline source.
  onceCache_.clear();
  if (shaderSource_->isPath()) {
    return load(shaderSource_->value);
  } else {
    return preprocessShader(".", shaderSource_->value);
  }
}
}  // namespace qrk
