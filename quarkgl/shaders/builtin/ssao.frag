#version 460 core
#pragma qrk_include < transforms.glsl>
#pragma qrk_include < window.frag>

in vec2 texCoords;

out float fragColor;

uniform sampler2D gPositionAO;
uniform sampler2D gNormalRoughness;
uniform sampler2D qrk_ssaoNoise;

#ifndef QRK_MAX_SSAO_KERNEL_SIZE
#define QRK_MAX_SSAO_KERNEL_SIZE 64
#endif

uniform float qrk_ssaoSampleRadius;
uniform float qrk_ssaoSampleBias;
uniform vec3 qrk_ssaoKernel[QRK_MAX_SSAO_KERNEL_SIZE];
uniform int qrk_ssaoKernelSize;

uniform mat4 projection;

void main() {
  ivec2 noiseSize = textureSize(qrk_ssaoNoise, /*lod=*/0);
  // Create a scale factor to tile the noise texture across the screen (this
  // relies on GL_REPEAT wrap mode).
  vec2 noiseScale =
      vec2(qrk_windowWidth / noiseSize.x, qrk_windowHeight / noiseSize.y);

  vec3 fragPos_viewSpace = texture(gPositionAO, texCoords).rgb;
  vec3 fragNormal_viewSpace = texture(gNormalRoughness, texCoords).rgb;
  vec3 noise_tangentSpace = texture(qrk_ssaoNoise, texCoords * noiseScale).rgb;

  // Treat the noise as the tangent. By doing this, we create an orthonormal
  // basis that is slightly tilted based on the noise. Note that since we're
  // using a random noise, this doesn't need to align with the surface UVs like
  // it would for normal mapping.
  // TODO: Is this correct to do?
  mat3 fragTBN_viewSpace =
      qrk_calculateTBN(fragNormal_viewSpace, noise_tangentSpace);

  // Calculate the occlusion factor.
  float occlusion = 0.0;
  for (int i = 0; i < qrk_ssaoKernelSize; ++i) {
    // Calculate sample position. Convert from tangent to view space.
    vec3 sampleOffset_viewSpace = fragTBN_viewSpace * qrk_ssaoKernel[i];
    vec3 samplePos_viewSpace =
        fragPos_viewSpace + sampleOffset_viewSpace * qrk_ssaoSampleRadius;

    // Now we transform the sample to screen space.
    vec4 samplePos_clipSpace = projection * vec4(samplePos_viewSpace, 1.0);
    samplePos_clipSpace /= samplePos_clipSpace.w;  // Perspective divide.
    // Transform from [-1, 1] to [0, 1] so that we can use them as tex coords.
    samplePos_clipSpace = samplePos_clipSpace * 0.5 + 0.5;

    // Finally we look up the sample depth.
    // TODO: What value does this have for texels that weren't drawn to?
    float sampleDepth = texture(gPositionAO, samplePos_clipSpace.xy).z;

    // Since we sampled via screen space, it's possible that the depth we found
    // isn't actually within the sample radius and is instead far behind the
    // surface, so we weigh the occlusion contribution based on the difference
    // in depths.
    float occlusionWeight = smoothstep(
        0.0, 1.0,
        qrk_ssaoSampleRadius / abs(samplePos_viewSpace.z - sampleDepth));
    // We use a bias to avoid visual issues.
    if (sampleDepth >= samplePos_viewSpace.z + qrk_ssaoSampleBias) {
      occlusion += occlusionWeight;
    }
  }

  // Normalize.
  occlusion /= qrk_ssaoKernelSize;

  // Output the inverse so that we can multiply it directly with lighting.
  fragColor = 1.0 - occlusion;
}
