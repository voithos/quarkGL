#version 330 core
#pragma qrk_include < core.frag >
#pragma qrk_include < standard_lights.frag >
#pragma qrk_include < depth.frag >
in vec2 texCoords;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

uniform QrkMaterial material;

void main() {
  vec3 normal = normalize(fragNormal);

  // Shade with normal lights.
  vec3 result = qrk_shadeAllLights(material, fragPos, normal, texCoords);

  // Add emissions.
  result += qrk_shadeEmission(material, fragPos, texCoords);

  fragColor = vec4(result, qrk_materialAlpha(material, texCoords));
}
