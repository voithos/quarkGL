#ifndef QUARKGL_AA_H_
#define QUARKGL_AA_H_

#include <qrk/shader.h>
#include <qrk/shader_primitives.h>

namespace qrk {

class FXAAShader : public ScreenShader {
 public:
  FXAAShader();
};

}  // namespace qrk

#endif