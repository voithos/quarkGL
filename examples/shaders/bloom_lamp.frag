#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < tone_mapping.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader with bloom for light sources.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
fs_in;

// The normal color of the fragment.
layout(location = 0) out vec4 fragColor;
// Only outputs if luminance > 1.
layout(location = 1) out vec4 brightColor;

uniform vec3 lightColor;

void main() {
  fragColor = vec4(lightColor, 1.0);

  // --- Bloom logic ---
  float luminance = qrk_luminance(fragColor.rgb);
  if (luminance > 1.0) {
    brightColor = vec4(fragColor.rgb, 1.0);
  } else {
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
