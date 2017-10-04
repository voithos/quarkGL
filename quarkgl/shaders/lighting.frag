#ifndef QUARKGL_LIGHTING_FRAG_
#define QUARKGL_LIGHTING_FRAG_

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

#ifndef QRK_MAX_EMISSION_TEXTURES
#define QRK_MAX_EMISSION_TEXTURES 1
#endif

struct QrkMaterial {
  sampler2D diffuse[QRK_MAX_DIFFUSE_TEXTURES];
  sampler2D specular[QRK_MAX_SPECULAR_TEXTURES];
  sampler2D emission[QRK_MAX_EMISSION_TEXTURES];
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
 * Calculate the Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 qrk_shadePhong(QrkMaterial material, vec3 lightAmbient, vec3 lightDiffuse,
                    vec3 lightSpecular, vec3 lightDir, vec3 viewDir,
                    vec3 normal, vec2 texCoords, float intensity) {
  vec3 result = vec3(0.0);

  // Ambient and diffuse components.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  for (int i = 0; i < QRK_MAX_DIFFUSE_TEXTURES; i++) {
    vec3 diffuseMap = vec3(texture(material.diffuse[i], texCoords));

    // Ambient component.
    result += lightAmbient * diffuseMap;

    // Diffuse component.
    result += (lightDiffuse * (diffuseIntensity * diffuseMap)) * intensity;
  }

  // Specular component.
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularIntensity =
      pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  for (int i = 0; i < QRK_MAX_SPECULAR_TEXTURES; i++) {
    vec3 specularMap = vec3(texture(material.specular[i], texCoords));
    result += (lightSpecular * (specularIntensity * specularMap)) * intensity;
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
  for (int i = 0; i < QRK_MAX_EMISSION_TEXTURES; i++) {
    result += vec3(texture(material.emission[i], texCoords)) * attenuation;
  }

  return result;
}
#endif
