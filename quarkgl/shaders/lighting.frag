#pragma once

#pragma qrk_include < gamma.frag>
#pragma qrk_include < normals.frag>

/** Core lighting structs and functions. */

struct QrkAttenuation {
  float constant;
  float linear;
  float quadratic;
};

#ifndef QRK_MAX_DIFFUSE_TEXTURES
#define QRK_MAX_DIFFUSE_TEXTURES 1
#endif

#ifndef QRK_MAX_SPECULAR_TEXTURES
#define QRK_MAX_SPECULAR_TEXTURES 1
#endif

#ifndef QRK_MAX_ROUGHNESS_TEXTURES
#define QRK_MAX_ROUGHNESS_TEXTURES 1
#endif

#ifndef QRK_MAX_METALLIC_TEXTURES
#define QRK_MAX_METALLIC_TEXTURES 1
#endif

#ifndef QRK_MAX_AO_TEXTURES
#define QRK_MAX_AO_TEXTURES 1
#endif

#ifndef QRK_MAX_EMISSION_TEXTURES
#define QRK_MAX_EMISSION_TEXTURES 1
#endif

struct QrkMaterial {
  // Material maps. Standard lighting logic only uses a single map, but
  // multiple maps are available in case user code wants to do something fancy.

  // TODO: Consider splitting out phong / PBR material properties.
  sampler2D diffuseMaps[QRK_MAX_DIFFUSE_TEXTURES];
  int diffuseCount;

  sampler2D specularMaps[QRK_MAX_SPECULAR_TEXTURES];
  int specularCount;

  sampler2D roughnessMaps[QRK_MAX_ROUGHNESS_TEXTURES];
  bool roughnessIsPacked[QRK_MAX_ROUGHNESS_TEXTURES];
  int roughnessCount;

  sampler2D metallicMaps[QRK_MAX_METALLIC_TEXTURES];
  bool metallicIsPacked[QRK_MAX_METALLIC_TEXTURES];
  int metallicCount;

  sampler2D aoMaps[QRK_MAX_AO_TEXTURES];
  bool aoIsPacked[QRK_MAX_AO_TEXTURES];
  int aoCount;

  sampler2D emissionMaps[QRK_MAX_EMISSION_TEXTURES];
  int emissionCount;

  sampler2D normalMap;
  bool hasNormalMap;

  // Ambient light factor.
  vec3 ambient;

  float shininess;

  // TODO: Add emissionFactor.
  QrkAttenuation emissionAttenuation;
};

struct QrkDirectionalLight {
  vec3 direction;

  vec3 diffuse;
  vec3 specular;
};

struct QrkPointLight {
  vec3 position;

  vec3 diffuse;
  vec3 specular;

  QrkAttenuation attenuation;
};

struct QrkSpotLight {
  vec3 position;
  vec3 direction;
  float innerAngle;
  float outerAngle;

  vec3 diffuse;
  vec3 specular;

  QrkAttenuation attenuation;
};

/** Extracts albedo from the material. */
vec3 qrk_extractAlbedo(QrkMaterial material, vec2 texCoords) {
  vec3 albedo = vec3(0.0);
  if (material.diffuseCount > 0) {
    albedo = texture(material.diffuseMaps[0], texCoords).rgb;
  }
  return albedo;
}

/** Extracts specular from the material. */
vec3 qrk_extractSpecular(QrkMaterial material, vec2 texCoords) {
  // In the absence of a specular map, we just calculate a half specular
  // component.
  vec3 specular = vec3(0.5);
  if (material.specularCount > 0) {
    // We only need a single channel. Sometimes we treat metallic maps as
    // specular maps, so extract from the blue channel in case the metallic map
    // is part of a packed roughness/metallic texture.
    specular = vec3(texture(material.specularMaps[0], texCoords).b);
  }
  return specular;
}

/** Extracts roughness from the material. */
float qrk_extractRoughness(QrkMaterial material, vec2 texCoords) {
  float roughness = 0.5;
  if (material.roughnessCount > 0) {
    if (material.roughnessIsPacked[0]) {
      // Part of a packed texture. Traditionally, roughness is the green
      // channel.
      roughness = texture(material.roughnessMaps[0], texCoords).g;
    } else {
      // Separate texture.
      roughness = texture(material.roughnessMaps[0], texCoords).r;
    }
  }
  return roughness;
}

/** Extracts metallic from the material. */
float qrk_extractMetallic(QrkMaterial material, vec2 texCoords) {
  float metallic = 0.0;
  if (material.metallicCount > 0) {
    if (material.metallicIsPacked[0]) {
      // Part of a packed texture. Traditionally, metallic is the blue channel.
      metallic = texture(material.metallicMaps[0], texCoords).b;
    } else {
      // Separate texture.
      metallic = texture(material.metallicMaps[0], texCoords).r;
    }
  }
  return metallic;
}

/**
 * Extracts the ambient occlusion from the material. This is a value that can be
 * directly multiplied with lighting, with 0 == fully occluded, and 1 == not at
 * all occluded.
 */
float qrk_extractAmbientOcclusion(QrkMaterial material, vec2 texCoords) {
  float ao = 1.0;
  if (material.aoCount > 0) {
    // We could check `material.aoIsPacked[0]` here, but even in packed textures
    // we assume that AO is in the red channel, so we can avoid that check. :)
    ao = texture(material.aoMaps[0], texCoords).r;
  }
  return ao;
}

/** Extracts emission from the material. */
vec3 qrk_extractEmission(QrkMaterial material, vec2 texCoords) {
  vec3 emission = vec3(0.0);
  if (material.emissionCount > 0) {
    emission = texture(material.emissionMaps[0], texCoords).rgb;
  }
  return emission;
}

/**
 * Calculate a material's final alpha based on its set of diffuse textures.
 */
float qrk_materialAlpha(QrkMaterial material, vec2 texCoords) {
  float sum = 0.0;
  for (int i = 0; i < material.diffuseCount; i++) {
    sum += texture(material.diffuseMaps[i], texCoords).a;
  }
  return min(sum, 1.0);
}

/**
 * Calculate attenuation based on fragment distance from a light source.
 * Returns a multipler that can be used in shading.
 */
float qrk_calcAttenuation(QrkAttenuation attenuation, float fragDist) {
  // Avoid dividing by zero.
  return 1.0 / max(attenuation.constant + attenuation.linear * fragDist +
                       attenuation.quadratic * (fragDist * fragDist),
                   1e-4);
}

/**
 * Calculate the directional intensity for a spotlight given the direction from
 * a fragment.
 */
float qrk_calcSpotLightIntensity(QrkSpotLight light, vec3 lightDir) {
  // Calculate cosine of the angle between the spotlight's direction vector and
  // the direction from the light to the current fragment.
  float theta = dot(lightDir, normalize(-light.direction));

  // Calculate the intensity based on fragment position, having zero intensity
  // when it falls outside the cone, partial intensity when it lies between
  // innerAngle and outerAngle, and full intensity when it falls in the cone.
  float innerAngleCosine = cos(light.innerAngle);
  float outerAngleCosine = cos(light.outerAngle);
  float epsilon = innerAngleCosine - outerAngleCosine;
  // Things outside the spotlight will have 0 intensity.
  return clamp((theta - outerAngleCosine) / epsilon, 0.0, 1.0);
}

/** ============================ Ambient ============================ **/

/** Calculate the ambient shading component. */
vec3 qrk_shadeAmbientDeferred(vec3 albedo, vec3 ambient, float ao) {
  return albedo * ambient * ao;
}

/** Calculate shading for ambient component based on the given material. */
vec3 qrk_shadeAmbient(QrkMaterial material, vec3 albedo, vec2 texCoords) {
  float ao = qrk_extractAmbientOcclusion(material, texCoords);
  return qrk_shadeAmbientDeferred(albedo, material.ambient, ao);
}

/** ============================ Emission ============================ **/

/** Calculate deferred shading for emission based on an emission color. */
vec3 qrk_shadeEmissionDeferred(vec3 emissionColor, vec3 fragPos_viewSpace,
                               QrkAttenuation emissionAttenuation) {
  // Calculate emission attenuation towards camera.
  float fragDist = length(fragPos_viewSpace);
  float attenuation = qrk_calcAttenuation(emissionAttenuation, fragDist);
  // Emission component.
  return emissionColor * attenuation;
}

/** Calculate shading for emission textures on the given material. */
vec3 qrk_shadeEmission(QrkMaterial material, vec3 fragPos_viewSpace,
                       vec2 texCoords) {
  vec3 emission = qrk_extractEmission(material, texCoords);
  return qrk_shadeEmissionDeferred(emission, fragPos_viewSpace,
                                   material.emissionAttenuation);
}

/** ============================ Normals ============================ **/

/**
 * Looks up a normal from the material, using the provided TBN matrix to
 * convert from tangent space to the target space, or returns a vertex normal
 * if no normal map is present.
 */
vec3 qrk_getNormal(QrkMaterial material, vec2 texCoords, mat3 TBN,
                   vec3 vertexNormal) {
  if (material.hasNormalMap) {
    return normalize(TBN * qrk_sampleNormalMap(material.normalMap, texCoords));
  } else {
    return normalize(vertexNormal);
  }
}

/** ============================ Shadows ============================ **/

/** Calculate a shadow bias based on the surface normal and light direction. */
float qrk_shadowBias(float minBias, float maxBias, vec3 normal, vec3 lightDir) {
  return max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
}

/** Sample from a shadow map using 9-texel percentage-closer filtering. */
float qrk_shadowSamplePCF(sampler2D shadowMap, vec2 shadowTexCoords,
                          float currentDepth, float bias) {
  float shadow = 0.0;
  vec2 texelOffset = 1.0 / textureSize(shadowMap, /*mip=*/0);
  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      float pcfDepth =
          texture(shadowMap, shadowTexCoords + vec2(x, y) * texelOffset).r;
      // Check whether in shadow.
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  return shadow / 9.0;
}

/**
 * Calculate whether the given fragment is in shadow.
 * Returns 1.0 if in shadow, 0.0 if not.
 */
float qrk_shadow(sampler2D shadowMap, vec4 fragPosLightSpace, float bias) {
  // Perform perspective divide.
  vec3 projectedPos = fragPosLightSpace.xyz / fragPosLightSpace.w;
  // Shift to the range 0..1 so that we can compare with depth.
  projectedPos = projectedPos * 0.5 + 0.5;
  // Check for out-of-frustum.
  if (projectedPos.z > 1.0) {
    // Assume not in shadow.
    return 0.0;
  }
  vec2 shadowTexCoords = projectedPos.xy;
  float currentDepth = projectedPos.z;
  return qrk_shadowSamplePCF(shadowMap, shadowTexCoords, currentDepth, bias);
}