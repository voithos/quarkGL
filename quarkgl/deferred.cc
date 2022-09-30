#include <qrk/deferred.h>

namespace qrk {

DeferredGeometryPassShader::DeferredGeometryPassShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/deferred.vert"),
             ShaderPath("quarkgl/shaders/builtin/deferred.frag")) {}

}  // namespace qrk
