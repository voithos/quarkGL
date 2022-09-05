#include <qrk/texture_registry.h>

namespace qrk {

void TextureRegistry::updateUniforms(Shader& shader) {
  nextTextureUnit_ = 0;
  lastAvailableUnits_.clear();

  for (auto source : textureSources_) {
    nextTextureUnit_ = source->bindTexture(nextTextureUnit_, shader);
  }
}

void TextureRegistry::pushUsageBlock() {
  lastAvailableUnits_.push_back(nextTextureUnit_);
}

void TextureRegistry::popUsageBlock() {
  if (lastAvailableUnits_.empty()) {
    throw TextureRegistryException("ERROR::TEXTURE_REGISTRY::POP");
  }
  nextTextureUnit_ = lastAvailableUnits_.back();
  lastAvailableUnits_.pop_back();
}

}  // namespace qrk
