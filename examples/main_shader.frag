#version 330 core
#pragma qrk_include <standard_lights.frag>
in vec2 texCoords;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

uniform float time;
uniform Material material;

void main() {
  vec3 normal = normalize(fragNormal);

  // Shade with normal lights.
  vec3 result = shadeAllLights(material, fragPos, normal, texCoords);

  // Add emissions.
  result += shadeEmission(material, fragPos, texCoords);

  fragColor = vec4(result, 1.0);
}
