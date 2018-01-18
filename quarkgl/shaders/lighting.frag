#ifndef QUARKGL_LIGHTING_FRAG_
#define QUARKGL_LIGHTING_FRAG_

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
 * Calculate the Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 qrk_shadePhong(QrkMaterial material, vec3 lightAmbient, vec3 lightDiffuse,
                    vec3 lightSpecular, vec3 lightDir, vec3 viewDir,
                    vec3 normal, vec2 texCoords, float intensity) {
  vec3 result = vec3(0.0);

  // Ambient and diffuse components.
  float diffuseAlphaSum = qrk_sumDiffuseAlpha(material, texCoords);
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  for (int i = 0; i < material.diffuseCount; i++) {
    vec4 diffuseMap = texture(material.diffuse[i], texCoords);
    float alphaRatio = qrk_optAlpha(diffuseMap.a) / diffuseAlphaSum;

    // Ambient component.
    result += lightAmbient * vec3(diffuseMap) * alphaRatio;

    // Diffuse component.
    result +=
        (lightDiffuse * (diffuseIntensity * vec3(diffuseMap) * alphaRatio)) *
        intensity;
  }

  // Specular component.
  float specularAlphaSum = qrk_sumSpecularAlpha(material, texCoords);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float specularIntensity =
      pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
  for (int i = 0; i < material.specularCount; i++) {
    vec4 specularMap = texture(material.specular[i], texCoords);
    float alphaRatio = qrk_optAlpha(specularMap.a) / specularAlphaSum;
    result +=
        (lightSpecular * (specularIntensity * vec3(specularMap) * alphaRatio)) *
        intensity;
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
                               vec3 fragPos, vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  return qrk_shadePhong(material, light.ambient, light.diffuse, light.specular,
                        lightDir, viewDir, normal, texCoords,
                        /* intensity */ 1.0);
}

/** Calculate shading for a point light source. */
vec3 qrk_shadePointLight(QrkMaterial material, QrkPointLight light,
                         vec3 fragPos, vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  vec3 result =
      qrk_shadePhong(material, light.ambient, light.diffuse, light.specular,
                     lightDir, viewDir, normal, texCoords, /* intensity */ 1.0);
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

  vec3 result =
      qrk_shadePhong(material, light.ambient, light.diffuse, light.specular,
                     lightDir, viewDir, normal, texCoords, intensity);
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
#endif
