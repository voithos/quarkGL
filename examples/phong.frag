#version 330 core
#pragma qrk_include < core.glsl >
#pragma qrk_include < standard_lights.frag >
#pragma qrk_include < depth.frag >

in VS_OUT {
  vec2 texCoords;
  vec3 fragPos;
  vec3 fragNormal;
}
fs_in;

out vec4 fragColor;

uniform QrkMaterial material;

void main() {
  vec3 normal = normalize(fs_in.fragNormal);

  // Shade with normal lights.
  vec3 result =
      qrk_shadeAllLights(material, fs_in.fragPos, normal, fs_in.texCoords);

  // Add emissions.
  result += qrk_shadeEmission(material, fs_in.fragPos, fs_in.texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, fs_in.texCoords));
  fragColor.rgb = qrk_gammaCorrect(fragColor.rgb);
}
