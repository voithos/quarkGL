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
#define QRK_MAX_DIFFUSE_TEXTURES 5
#endif

#ifndef QRK_MAX_SPECULAR_TEXTURES
#define QRK_MAX_SPECULAR_TEXTURES 5
#endif

#ifndef QRK_MAX_EMISSION_TEXTURES
#define QRK_MAX_EMISSION_TEXTURES 5
#endif

struct QrkMaterial {
  // Material maps. Standard lighting logic only uses a single map, but
  // multiple maps are available in case user code wants to do something fancy.

  sampler2D diffuseMaps[QRK_MAX_DIFFUSE_TEXTURES];
  int diffuseCount;
  sampler2D specularMaps[QRK_MAX_SPECULAR_TEXTURES];
  int specularCount;
  sampler2D emissionMaps[QRK_MAX_EMISSION_TEXTURES];
  int emissionCount;
  sampler2D normalMap;
  bool hasNormalMap;

  // Ambient light factor.
  vec3 ambient;

  float shininess;

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

/**
 * Calculate the deferred Blinn-Phong shading model with ambient, diffuse, and
 * specular components, along with direct material colors. Does not include
 * attenuation.
 */
vec3 qrk_shadeBlinnPhongDeferred(vec3 albedo, vec3 specular, vec3 ambient,
                                 float shininess, vec3 lightDiffuse,
                                 vec3 lightSpecular, vec3 lightDir,
                                 vec3 viewDir, vec3 normal, float intensity,
                                 float shadow, float ao) {
  vec3 result = vec3(0.0);
  float shadowMultiplier = 1.0 - shadow;

  // Ambient and diffuse components.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  // Ambient component. Don't include shadow calculation here, since it
  // shouldn't affect ambient light, but do include AO.
  result += ambient * albedo * ao;

  // Diffuse component.
  result +=
      lightDiffuse * diffuseIntensity * albedo * intensity * shadowMultiplier;

  // Specular component.
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float specularIntensity = pow(max(dot(normal, halfwayDir), 0.0), shininess);
  result += lightSpecular * specularIntensity * specular * intensity *
            shadowMultiplier;

  return result;
}

/** Extracts albedo from the material. */
vec3 qrk_extractAlbedo(QrkMaterial material, vec2 texCoords) {
  vec3 albedo = vec3(0.0);
  if (material.diffuseCount > 0) {
    albedo = vec3(texture(material.diffuseMaps[0], texCoords));
  }
  return albedo;
}

/** Extracts specular from the material. */
vec3 qrk_extractSpecular(QrkMaterial material, vec2 texCoords) {
  // In the absence of a specular map, we just calculate a half specular
  // component.
  vec3 specular = vec3(0.5);
  if (material.specularCount > 0) {
    specular = vec3(texture(material.specularMaps[0], texCoords));
  }
  return specular;
}

/** Extracts emission from the material. */
vec3 qrk_extractEmission(QrkMaterial material, vec2 texCoords) {
  vec3 emission = vec3(0.0);
  if (material.emissionCount > 0) {
    emission = vec3(texture(material.emissionMaps[0], texCoords));
  }
  return emission;
}

/**
 * Calculate the Blinn-Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 qrk_shadeBlinnPhong(QrkMaterial material, vec3 lightDiffuse,
                         vec3 lightSpecular, vec3 lightDir, vec3 viewDir,
                         vec3 normal, vec2 texCoords, float intensity,
                         float shadow, float ao) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  vec3 specular = qrk_extractSpecular(material, texCoords);
  // Use the deferred calculations, but do it directly, so it's not deferred.
  // :)
  return qrk_shadeBlinnPhongDeferred(
      albedo, specular, material.ambient, material.shininess, lightDiffuse,
      lightSpecular, lightDir, viewDir, normal, intensity, shadow, ao);
}

/**
 * Calculate attenuation based on fragment distance from a light source.
 * Returns a multipler that can be used in shading.
 */
float qrk_calcAttenuation(QrkAttenuation attenuation, float fragDist) {
  return 1.0 / (attenuation.constant + attenuation.linear * fragDist +
                attenuation.quadratic * (fragDist * fragDist));
}

/** Calculate shading for a directional light source using deferred data. */
vec3 qrk_shadeDirectionalLightDeferred(vec3 albedo, vec3 specular, vec3 ambient,
                                       float shininess,
                                       QrkDirectionalLight light, vec3 fragPos,
                                       vec3 normal, float shadow, float ao) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  return qrk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, /*intensity=*/1.0, shadow, ao);
}

/** Calculate shading for a directional light source. */
vec3 qrk_shadeDirectionalLight(QrkMaterial material, QrkDirectionalLight light,
                               vec3 fragPos, vec3 normal, vec2 texCoords,
                               float shadow, float ao) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  vec3 specular = qrk_extractSpecular(material, texCoords);
  return qrk_shadeDirectionalLightDeferred(albedo, specular, material.ambient,
                                           material.shininess, light, fragPos,
                                           normal, shadow, ao);
}

/** Calculate shading for a point light source using deferred data. */
vec3 qrk_shadePointLightDeferred(vec3 albedo, vec3 specular, vec3 ambient,
                                 float shininess, QrkPointLight light,
                                 vec3 fragPos, vec3 normal, float ao) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  vec3 result = qrk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, /*intensity=*/1.0, /*shadow=*/0.0, ao);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a point light source. */
vec3 qrk_shadePointLight(QrkMaterial material, QrkPointLight light,
                         vec3 fragPos, vec3 normal, vec2 texCoords, float ao) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  vec3 specular = qrk_extractSpecular(material, texCoords);
  return qrk_shadePointLightDeferred(albedo, specular, material.ambient,
                                     material.shininess, light, fragPos, normal,
                                     ao);
}

/** Calculate shading for a spot light source using deferred data. */
vec3 qrk_shadeSpotLightDeferred(vec3 albedo, vec3 specular, vec3 ambient,
                                float shininess, QrkSpotLight light,
                                vec3 fragPos, vec3 normal, float ao) {
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

  vec3 result = qrk_shadeBlinnPhongDeferred(
      albedo, specular, ambient, shininess, light.diffuse, light.specular,
      lightDir, viewDir, normal, intensity, /*shadow=*/0.0, ao);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a spot light source. */
vec3 qrk_shadeSpotLight(QrkMaterial material, QrkSpotLight light, vec3 fragPos,
                        vec3 normal, vec2 texCoords, float ao) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  vec3 specular = qrk_extractSpecular(material, texCoords);
  return qrk_shadeSpotLightDeferred(albedo, specular, material.ambient,
                                    material.shininess, light, fragPos, normal,
                                    ao);
}

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

/** =========================== Deferred Shading =========================== **/

/** Return the sum color from the material's diffuse maps. */
vec3 qrk_sumDiffuseColor(QrkMaterial material, vec2 texCoords) {
  vec3 sum = vec3(0);
  for (int i = 0; i < material.diffuseCount; i++) {
    sum += texture(material.diffuseMaps[i], texCoords).rgb;
  }
  return sum;
}

/** Return the sum color from the material's specular maps, or 1.0 if there are
 * no maps. */
vec3 qrk_sumSpecularColor(QrkMaterial material, vec2 texCoords) {
  if (material.specularCount == 0) {
    return vec3(0.5);
  }
  vec3 sum = vec3(0);
  for (int i = 0; i < material.specularCount; i++) {
    sum += texture(material.specularMaps[i], texCoords).rgb;
  }
  return sum;
}

/** Return the sum color from the material's emission maps. */
vec3 qrk_sumEmissionColor(QrkMaterial material, vec2 texCoords) {
  vec3 sum = vec3(0);
  for (int i = 0; i < material.emissionCount; i++) {
    sum += texture(material.emissionMaps[i], texCoords).rgb;
  }
  return sum;
}

#endif
