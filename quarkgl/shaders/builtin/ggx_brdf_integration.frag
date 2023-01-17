#version 460 core
#pragma qrk_include < constants.glsl>
#pragma qrk_include < pbr.frag>
#pragma qrk_include < pbr_sampling.glsl>
#pragma qrk_include < random.glsl>

in vec2 texCoords;

out vec3 fragColor;

uniform uint qrk_numSamples;

vec2 integrateGGXBrdf(float NdotV, float roughness) {
  // Reconstruct viewDir.
  vec3 viewDir;
  viewDir.x = sqrt(1.0 - NdotV * NdotV);
  viewDir.y = 0.0;
  viewDir.z = NdotV;

  float outputScale = 0.0;
  float outputBias = 0.0;

  vec3 normal = vec3(0.0, 0.0, 1.0);

  for (uint i = 0u; i < qrk_numSamples; ++i) {
    vec2 Xi = qrk_hammersley(i, qrk_numSamples);
    vec3 halfVector = qrk_importanceSampleGGX(normal, roughness, Xi);
    // Reverse engineer the lightDir from the sampled halfVector.
    vec3 lightDir =
        normalize(2.0 * dot(viewDir, halfVector) * halfVector - viewDir);

    // Shortcut because we know the normal.
    float NdotL = clamp(lightDir.z, 0.0, 1.0);
    if (NdotL > 0.0) {
      float NdotH = clamp(halfVector.z, 0.0, 1.0);
      float HdotV = clamp(dot(halfVector, viewDir), 0.0, 1.0);

      float a = roughness * roughness;
      float V = qrk_visibilitySmithGGXCorrelated(NdotV, NdotL, a);
      // Calculate geometric visibility term. Simplified from a multiplication
      // by 4 * NdotV * NdotL, which normally gets cancelled in the correlated
      // visibility term.
      // https://google.github.io/filament/Filament.md.html#toc5.3.4.2.2
      float GVis = (4.0 * V * HdotV * NdotL) / NdotH;
      float Fc = pow(1.0 - HdotV, 5.0);

      outputScale += (1.0 - Fc) * GVis;
      outputBias += Fc * GVis;
    }
  }
  outputScale /= float(qrk_numSamples);
  outputBias /= float(qrk_numSamples);
  return vec2(outputScale, outputBias);
}

void main() {
  fragColor = vec3(integrateGGXBrdf(texCoords.x, texCoords.y), 0.0);
}