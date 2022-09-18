#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < tone_mapping.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader with bloom. Uses multiple render targets to output
// bloom info.

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

uniform QrkMaterial material;
uniform bool skipGamma;

void main() {
  vec3 normal = normalize(fs_in.fragNormal);

  // Shade with normal lights.
  vec3 result =
      qrk_shadeAllLights(material, fs_in.fragPos, normal, fs_in.texCoords);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  if (!skipGamma) {
    fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
  }

  // --- Bloom logic ---
  float luminance = qrk_luminance(fragColor.rgb);
  if (luminance > 1.0) {
    brightColor = vec4(fragColor.rgb, 1.0);
  } else {
    brightColor = vec4(0.0, 0.0, 0.0, 1.0);
  }
}
