#include <qrk/aa.h>

namespace qrk {

FXAAShader::FXAAShader()
    : ScreenShader(ShaderPath("quarkgl/shaders/builtin/fxaa.frag")) {}

}  // namespace qrk