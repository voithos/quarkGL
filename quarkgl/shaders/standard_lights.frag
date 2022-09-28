#ifndef QUARKGL_STANDARD_LIGHTS_FRAG_
#define QUARKGL_STANDARD_LIGHTS_FRAG_
#pragma qrk_include < lighting.frag>

#ifndef QRK_MAX_DIRECTIONAL_LIGHTS
#define QRK_MAX_DIRECTIONAL_LIGHTS 10
#endif
uniform QrkDirectionalLight qrk_directionalLights[QRK_MAX_DIRECTIONAL_LIGHTS];
uniform int qrk_directionalLightCount;

#ifndef QRK_MAX_POINT_LIGHTS
#define QRK_MAX_POINT_LIGHTS 10
#endif
uniform QrkPointLight qrk_pointLights[QRK_MAX_POINT_LIGHTS];
uniform int qrk_pointLightCount;

#ifndef QRK_MAX_SPOT_LIGHTS
#define QRK_MAX_SPOT_LIGHTS 10
#endif
uniform QrkSpotLight qrk_spotLights[QRK_MAX_SPOT_LIGHTS];
uniform int qrk_spotLightCount;

/** Calculate shading from all active directional lights. */
vec3 qrk_shadeAllDirectionalLights(QrkMaterial material, vec3 fragPos,
                                   vec3 normal, vec2 texCoords, float shadow) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_directionalLightCount; i++) {
    result += qrk_shadeDirectionalLight(material, qrk_directionalLights[i],
                                        fragPos, normal, texCoords, shadow);
  }
  return result;
}

/** Calculate shading from all active directional lights using deferred data. */
vec3 qrk_shadeAllDirectionalLightsDeferred(vec3 albedo, vec3 specular,
                                           float shininess, vec3 fragPos,
                                           vec3 normal, float shadow) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_directionalLightCount; i++) {
    result += qrk_shadeDirectionalLightDeferred(albedo, specular, shininess,
                                                qrk_directionalLights[i],
                                                fragPos, normal, shadow);
  }
  return result;
}

/** Calculate shading from all active point lights. */
vec3 qrk_shadeAllPointLights(QrkMaterial material, vec3 fragPos, vec3 normal,
                             vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_pointLightCount; i++) {
    result += qrk_shadePointLight(material, qrk_pointLights[i], fragPos, normal,
                                  texCoords);
  }
  return result;
}

/** Calculate shading from all active point lights using deferred data. */
vec3 qrk_shadeAllPointLightsDeferred(vec3 albedo, vec3 specular,
                                     float shininess, vec3 fragPos,
                                     vec3 normal) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_pointLightCount; i++) {
    result += qrk_shadePointLightDeferred(albedo, specular, shininess,
                                          qrk_pointLights[i], fragPos, normal);
  }
  return result;
}

/** Calculate shading from all active spot lights. */
vec3 qrk_shadeAllSpotLights(QrkMaterial material, vec3 fragPos, vec3 normal,
                            vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_spotLightCount; i++) {
    result += qrk_shadeSpotLight(material, qrk_spotLights[i], fragPos, normal,
                                 texCoords);
  }
  return result;
}

/** Calculate shading from all active spot lights using deferred data. */
vec3 qrk_shadeAllSpotLightsDeferred(vec3 albedo, vec3 specular, float shininess,
                                    vec3 fragPos, vec3 normal) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_spotLightCount; i++) {
    result += qrk_shadeSpotLightDeferred(albedo, specular, shininess,
                                         qrk_spotLights[i], fragPos, normal);
  }
  return result;
}

/** Calculate shading from all light sources, except emission textures. */
vec3 qrk_shadeAllLights(QrkMaterial material, vec3 fragPos, vec3 normal,
                        vec2 texCoords, float shadow) {
  vec3 directional = qrk_shadeAllDirectionalLights(material, fragPos, normal,
                                                   texCoords, shadow);
  vec3 point = qrk_shadeAllPointLights(material, fragPos, normal, texCoords);
  vec3 spot = qrk_shadeAllSpotLights(material, fragPos, normal, texCoords);
  return directional + point + spot;
}

vec3 qrk_shadeAllLights(QrkMaterial material, vec3 fragPos, vec3 normal,
                        vec2 texCoords) {
  return qrk_shadeAllLights(material, fragPos, normal, texCoords,
                            /*shadow=*/0.0);
}

/** Calculate shading from all light sources, except emission textures, using
 * deferred data. */
vec3 qrk_shadeAllLightsDeferred(vec3 albedo, vec3 specular, float shininess,
                                vec3 fragPos, vec3 normal, float shadow) {
  // vec3 directional = qrk_shadeAllDirectionalLightsDeferred(
  // albedo, specular, shininess, fragPos, normal, shadow);
  vec3 point = qrk_shadeAllPointLightsDeferred(albedo, specular, shininess,
                                               fragPos, normal);
  // vec3 spot = qrk_shadeAllSpotLightsDeferred(albedo, specular, shininess,
  // fragPos, normal);
  return point;
}

vec3 qrk_shadeAllLightsDeferred(vec3 albedo, vec3 specular, float shininess,
                                vec3 fragPos, vec3 normal) {
  return qrk_shadeAllLightsDeferred(albedo, specular, shininess, fragPos,
                                    normal,
                                    /*shadow=*/0.0);
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

#endif
