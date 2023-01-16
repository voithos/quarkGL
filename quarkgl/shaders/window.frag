#pragma once

// TODO: This file should be .glsl

uniform float qrk_deltaTime;
// TODO: Replace these uniforms with a vec2.
uniform int qrk_windowWidth;
uniform int qrk_windowHeight;

bool qrk_isWindowLeftHalf() { return gl_FragCoord.x < (qrk_windowWidth / 2); }
bool qrk_isWindowRightHalf() { return gl_FragCoord.x >= (qrk_windowWidth / 2); }
bool qrk_isWindowBottomHalf() {
  return gl_FragCoord.y < (qrk_windowHeight / 2);
}
bool qrk_isWindowTopHalf() { return gl_FragCoord.y >= (qrk_windowHeight / 2); }
