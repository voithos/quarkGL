#version 460 core
#pragma qrk_include < constants.glsl>

out vec4 fragColor;

in vec3 cubemapCoords;

uniform sampler2D qrk_equirectMap;

const vec2 invMapping = vec2(1.0 / (2.0 * PI), 1.0 / PI);

// Converts a normalized cubemap direction vector to the equivalent UV
// coordinates for a spherically-mapped equirect.
vec2 sampleSphericalMap(vec3 v) {
  vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
  // Map from [-PI, PI] to [0, 1] for U and [-PI/2, PI/2] to [0, 1] for V.
  uv *= invMapping;
  uv += 0.5;
  return uv;
}

void main() {
  vec2 uv = sampleSphericalMap(normalize(cubemapCoords));
  fragColor = texture(qrk_equirectMap, uv);
}