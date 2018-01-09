#version 330 core
#pragma qrk_include < post_processing.frag >
in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

// TODO: Allow other post_processing methods from command line.
void main() { fragColor = qrk_edgeKernel(screenTexture, texCoords); }
