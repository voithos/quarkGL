#ifndef QUARKGL_WINDOW_FRAG_
#define QUARKGL_WINDOW_FRAG_

uniform float qrk_deltaTime;
uniform int qrk_windowWidth;
uniform int qrk_windowHeight;

bool qrk_isWindowLeftHalf() { return gl_FragCoord.x < (qrk_windowWidth / 2); }
bool qrk_isWindowRightHalf() { return gl_FragCoord.x >= (qrk_windowWidth / 2); }
bool qrk_isWindowBottomHalf() {
  return gl_FragCoord.y < (qrk_windowHeight / 2);
}
bool qrk_isWindowTopHalf() { return gl_FragCoord.y >= (qrk_windowHeight / 2); }

#endif
