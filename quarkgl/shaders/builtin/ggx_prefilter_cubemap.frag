#version 460 core
#pragma qrk_include < constants.glsl>
#pragma qrk_include < pbr_sampling.glsl>
#pragma qrk_include < random.glsl>

out vec4 fragColor;

in vec3 cubemapCoords;

uniform samplerCube qrk_environmentMap;
uniform float qrk_roughness;
uniform uint qrk_numSamples;

void main() {
  // We orient the sampling hemisphere in the direction of the cubemap sample
  // vector.
  vec3 normal = normalize(cubemapCoords);
  // Simplifying assumption, we treat the sample output dir as the view dir.
  vec3 viewDir = normal;

  float totalWeight = 0.0;
  vec3 prefilteredColor = vec3(0.0);
  for (uint i = 0u; i < qrk_numSamples; ++i) {
    vec2 Xi = qrk_hammersley(i, qrk_numSamples);
    vec3 halfVector = qrk_importanceSampleGGX(normal, qrk_roughness, Xi);
    // Reverse engineer the lightDir from the sampled halfVector.
    vec3 lightDir =
        normalize(2.0 * dot(viewDir, halfVector) * halfVector - viewDir);
    float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
    // Make sure we ignore light directions that are behind the "virtual"
    // surface.
    if (NdotL > 0.0) {
      prefilteredColor += texture(qrk_environmentMap, lightDir).rgb * NdotL;
      totalWeight += NdotL;
    }
  }
  prefilteredColor = prefilteredColor / totalWeight;
  fragColor = vec4(prefilteredColor, 1.0);
}