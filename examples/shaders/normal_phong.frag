#version 460 core
#pragma qrk_include < core.glsl>
#pragma qrk_include < standard_lights.frag>
#pragma qrk_include < depth.frag>

// An example fragment shader with Phong illumination.

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;
uniform sampler2D normalMap;

void main() {
  // Lookup normal and map from color components [0..1] to vector components
  // [-1..1].
  vec3 normal = normalize(texture(normalMap, fs_in.texCoords).rgb * 2.0 - 1.0);

  // Shade with normal lights.
  vec3 result =
      qrk_shadeAllLights(material, fs_in.fragPos, normal, fs_in.texCoords);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
