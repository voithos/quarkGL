#version 460 core
#pragma qrk_include < constants.glsl>

out vec4 fragColor;

in vec3 cubemapCoords;

uniform samplerCube qrk_environmentMap;
uniform float qrk_hemisphereSampleDelta;

void main() {
  // We orient the sampling hemisphere in the direction of the cubemap sample
  // vector.
  vec3 normal = normalize(cubemapCoords);
  vec3 irradiance = vec3(0.0);

  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, normal));
  up = normalize(cross(normal, right));

  mat3 TBN = mat3(right, up, normal);

  int numSamples = 0;
  // Sample the hemisphere via spherical coordinates
  for (float phi = 0.0; phi < 2.0 * PI; phi += qrk_hemisphereSampleDelta) {
    for (float theta = 0.0; theta < 0.5 * PI;
         theta += qrk_hemisphereSampleDelta) {
      // Spherical to cartesian (in tangent space).
      vec3 tangentSample =
          vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
      vec3 sampleDir = TBN * tangentSample;

      irradiance +=
          texture(qrk_environmentMap, sampleDir).rgb * cos(theta) * sin(theta);
      ++numSamples;
    }
  }
  irradiance = (PI * irradiance) / float(numSamples);

  fragColor = vec4(irradiance, 1.0);
}