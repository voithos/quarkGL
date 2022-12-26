#version 460 core
#pragma qrk_include < post_processing.frag>
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;
uniform bool horizontal;

void main() {
  fragColor = qrk_gaussianBlurOnePass(screenTexture, texCoords, horizontal);
}
