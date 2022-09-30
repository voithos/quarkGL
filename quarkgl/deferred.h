#ifndef QUARKGL_DEFERRED_H_
#define QUARKGL_DEFERRED_H_

#include <qrk/exceptions.h>
#include <qrk/framebuffer.h>
#include <qrk/shader.h>

#include <glm/glm.hpp>

namespace qrk {

class DeferredShadingException : public QuarkException {
  using QuarkException::QuarkException;
};

class DeferredGeometryPassShader : public Shader {
 public:
  DeferredGeometryPassShader();
};

}  // namespace qrk

#endif
