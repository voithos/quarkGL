#ifndef QUARKGL_LIGHTING_FRAG_
#define QUARKGL_LIGHTING_FRAG_

#pragma qrk_include < gamma.frag>
#pragma qrk_include < normals.frag>

struct QrkAttenuation {
  float constant;
  float linear;
  float quadratic;
};

#ifndef QRK_MAX_DIFFUSE_TEXTURES
#define QRK_MAX_DIFFUSE_TEXTURES 10
#endif

#ifndef QRK_MAX_SPECULAR_TEXTURES
#define QRK_MAX_SPECULAR_TEXTURES 10
#endif

#ifndef QRK_MAX_EMISSION_TEXTURES
#define QRK_MAX_EMISSION_TEXTURES 10
#endif

struct QrkMaterial {
  sampler2D diffuse[QRK_MAX_DIFFUSE_TEXTURES];
  int diffuseCount;
  sampler2D specular[QRK_MAX_SPECULAR_TEXTURES];
  int specularCount;
  sampler2D emission[QRK_MAX_EMISSION_TEXTURES];
  int emissionCount;
  sampler2D normal;
  bool hasNormalMap;

  float shininess;

  QrkAttenuation emissionAttenuation;
};

struct QrkDirectionalLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct QrkPointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  QrkAttenuation attenuation;
};

struct QrkSpotLight {
  vec3 position;
  vec3 direction;
  float innerAngle;
  float outerAngle;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  QrkAttenuation attenuation;
};

/**
 * Return a transparency value for weighing.
 * If the alpha is nonzero, return it directly, otherwise return 1.0 (fully
 * opaque). This is useful for textures that don't have an alpha layer.
 * TODO: This doesn't make it possible for textures to have 0 transparency
 * pixels properly. While it's questionable whether that is useful, a
 * potentially better solution might be to pass in a "has alpha" flag for each
 * texture as part of the material.
 */
float qrk_optAlpha(float alpha) { return alpha + (1.0 - sign(alpha)) * 1.0; }

/** Return the sum of transparency values for all diffuse textures. */
float qrk_sumDiffuseAlpha(QrkMaterial material, vec2 texCoords) {
  float sum = 0;
  for (int i = 0; i < material.diffuseCount; i++) {
    sum += qrk_optAlpha(texture(material.diffuse[i], texCoords).a);
  }
  return sum;
}

/** Return the sum of transparency values for all specular textures. */
float qrk_sumSpecularAlpha(QrkMaterial material, vec2 texCoords) {
  float sum = 0;
  for (int i = 0; i < material.specularCount; i++) {
    sum += qrk_optAlpha(texture(material.specular[i], texCoords).a);
  }
  return sum;
}

/** Return the sum of transparency values for all emission textures. */
float qrk_sumEmissionAlpha(QrkMaterial material, vec2 texCoords) {
  float sum = 0;
  for (int i = 0; i < material.emissionCount; i++) {
    sum += qrk_optAlpha(texture(material.emission[i], texCoords).a);
  }
  return sum;
}

/**
 * Calculate the Blinn-Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 qrk_shadeBlinnPhong(QrkMaterial material, vec3 lightAmbient,
                         vec3 lightDiffuse, vec3 lightSpecular, vec3 lightDir,
                         vec3 viewDir, vec3 normal, vec2 texCoords,
                         float intensity, float shadow) {
  vec3 result = vec3(0.0);
  float shadowMultiplier = 1.0 - shadow;

  // Ambient and diffuse components.
  float diffuseAlphaSum = qrk_sumDiffuseAlpha(material, texCoords);
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  for (int i = 0; i < material.diffuseCount; i++) {
    vec4 diffuseMap = texture(material.diffuse[i], texCoords);
    float alphaRatio = qrk_optAlpha(diffuseMap.a) / diffuseAlphaSum;

    // Ambient component. Don't include shadow calculation here, since it
    // shouldn't affect ambient light.
    result += lightAmbient * vec3(diffuseMap) * alphaRatio;

    // Diffuse component.
    result +=
        (lightDiffuse * (diffuseIntensity * vec3(diffuseMap) * alphaRatio)) *
        intensity * shadowMultiplier;
  }

  // Specular component.
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float specularIntensity =
      pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
  // In the absence of a specular map, we just calculate the full specular
  // component without diminishing it by a map lookup.
  if (material.specularCount == 0) {
    result += lightSpecular * specularIntensity * intensity * shadowMultiplier;
  } else {
    // Specular maps present.
    float specularAlphaSum = qrk_sumSpecularAlpha(material, texCoords);
    for (int i = 0; i < material.specularCount; i++) {
      vec4 specularMap = texture(material.specular[i], texCoords);
      float alphaRatio = qrk_optAlpha(specularMap.a) / specularAlphaSum;
      result += (lightSpecular *
                 (specularIntensity * vec3(specularMap) * alphaRatio)) *
                intensity * shadowMultiplier;
    }
  }

  return result;
}

/**
 * Calculate attenuation based on fragment distance from a light source.
 * Returns a multipler that can be used in shading.
 */
float qrk_calcAttenuation(QrkAttenuation attenuation, float fragDist) {
  return 1.0 / (attenuation.constant + attenuation.linear * fragDist +
                attenuation.quadratic * (fragDist * fragDist));
}

/** Calculate shading for a directional light source. */
vec3 qrk_shadeDirectionalLight(QrkMaterial material, QrkDirectionalLight light,
                               vec3 fragPos, vec3 normal, vec2 texCoords,
                               float shadow) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  return qrk_shadeBlinnPhong(material, light.ambient, light.diffuse,
                             light.specular, lightDir, viewDir, normal,
                             texCoords,
                             /*intensity=*/1.0, shadow);
}

/** Calculate shading for a point light source. */
vec3 qrk_shadePointLight(QrkMaterial material, QrkPointLight light,
                         vec3 fragPos, vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  vec3 result = qrk_shadeBlinnPhong(
      material, light.ambient, light.diffuse, light.specular, lightDir, viewDir,
      normal, texCoords, /*intensity=*/1.0, /*shadow=*/0.0);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a spot light source. */
vec3 qrk_shadeSpotLight(QrkMaterial material, QrkSpotLight light, vec3 fragPos,
                        vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(light.position - fragPos);

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
  float intensity = clamp((theta - outerAngleCosine) / epsilon, 0.0, 1.0);

  // The rest is normal shading.
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  vec3 result = qrk_shadeBlinnPhong(material, light.ambient, light.diffuse,
                                    light.specular, lightDir, viewDir, normal,
                                    texCoords, intensity, /*shadow=*/0.0);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for emission textures on the given material. */
vec3 qrk_shadeEmission(QrkMaterial material, vec3 fragPos, vec2 texCoords) {
  vec3 result = vec3(0.0);

  // Calculate emission attenuation towards camera.
  float fragDist = length(fragPos);
  float attenuation =
      qrk_calcAttenuation(material.emissionAttenuation, fragDist);

  // Emission component.
  float emissionAlphaSum = qrk_sumEmissionAlpha(material, texCoords);
  for (int i = 0; i < material.emissionCount; i++) {
    vec4 emissionMap = texture(material.emission[i], texCoords);
    float alphaRatio = qrk_optAlpha(emissionMap.a) / emissionAlphaSum;
    result += vec3(emissionMap) * alphaRatio * attenuation;
  }

  return result;
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
    return normalize(TBN * qrk_sampleNormalMap(material.normal, texCoords));
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

#endif
