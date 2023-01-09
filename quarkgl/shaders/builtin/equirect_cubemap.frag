#version 460 core
out vec4 fragColor;

in vec3 cubemapCoords;

uniform sampler2D qrk_equirectMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
// Converts a normalized cubemap direction vector to the equivalent UV
// coordinates for a spherically-mapped equirect.
vec2 sampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  uv *= invAtan;
  uv += 0.5;
  return uv;
}

void main() {
  vec2 uv = sampleSphericalMap(normalize(cubemapCoords));
  fragColor = texture(qrk_equirectMap, uv);
}