#ifndef QUARKGL_TEXTURE_REGISTRY_H_
#define QUARKGL_TEXTURE_REGISTRY_H_

#include <qrk/exceptions.h>
#include <qrk/shader.h>
#include <qrk/texture.h>

#include <string>
#include <vector>

namespace qrk {

class TextureRegistryException : public QuarkException {
  using QuarkException::QuarkException;
};

class TextureSource {
 public:
  // Binds one or more textures to the next available texture unit, and
  // assigns shader uniforms. It must return the next texture unit that can be
  // used.
  virtual unsigned int bindTexture(unsigned int nextTextureUnit,
                                   Shader& shader) = 0;
};

// A manager of "texture-like" objects, in relation to how they are rendered.
// Acts as a uniform source that also binds any texture sources registered with
// it.
//
// Rendering code should set up any textures that won't change between draw
// calls (such as shadow maps) as part of a TextureSource added to this
// registry. Then for each draw call, code should push a usage block, call
// getNextTextureUnit repeatedly to set up texture, and then pop once done.
class TextureRegistry : public UniformSource {
 public:
  virtual ~TextureRegistry() = default;

  void addTextureSource(std::shared_ptr<TextureSource> source) {
    textureSources_.push_back(source);
  }
  void updateUniforms(Shader& shader) override;

  // Returns the next available texture unit, and increments the number.
  unsigned int getNextTextureUnit() { return nextTextureUnit_++; }

  // Remembers the next available texture unit, for a block of usage (such as a
  // draw call).
  void pushUsageBlock();

  // Resets the next available texture unit to the one at the last
  // pushUsageBlock call.
  void popUsageBlock();

 private:
  unsigned int nextTextureUnit_ = 0;
  std::vector<unsigned int> lastAvailableUnits_;
  std::vector<std::shared_ptr<TextureSource>> textureSources_;
};

}  // namespace qrk

#endif
