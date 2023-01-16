#pragma once

#pragma qrk_include < lighting.frag>
#pragma qrk_include < phong.frag>
#pragma qrk_include < standard_lights.frag>

/** Calculate shading from all active directional lights. */
vec3 qrk_shadeAllDirectionalLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                             vec3 normal, vec2 texCoords,
                                             float shadow, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_directionalLightCount; i++) {
    result += qrk_shadeDirectionalLightBlinnPhong(
        material, qrk_directionalLights[i], fragPos, normal, texCoords, shadow,
        ao);
  }
  return result;
}

/** Calculate shading from all active directional lights using deferred data. */
vec3 qrk_shadeAllDirectionalLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                                     vec3 ambient,
                                                     float shininess,
                                                     vec3 fragPos, vec3 normal,
                                                     float shadow, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_directionalLightCount; i++) {
    result += qrk_shadeDirectionalLightBlinnPhongDeferred(
        albedo, specular, ambient, shininess, qrk_directionalLights[i], fragPos,
        normal, shadow, ao);
  }
  return result;
}

/** Calculate shading from all active point lights. */
vec3 qrk_shadeAllPointLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                       vec3 normal, vec2 texCoords, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_pointLightCount; i++) {
    result += qrk_shadePointLightBlinnPhong(material, qrk_pointLights[i],
                                            fragPos, normal, texCoords, ao);
  }
  return result;
}

/** Calculate shading from all active point lights using deferred data. */
vec3 qrk_shadeAllPointLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                               vec3 ambient, float shininess,
                                               vec3 fragPos, vec3 normal,
                                               float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_pointLightCount; i++) {
    result += qrk_shadePointLightBlinnPhongDeferred(
        albedo, specular, ambient, shininess, qrk_pointLights[i], fragPos,
        normal, ao);
  }
  return result;
}

/** Calculate shading from all active spot lights. */
vec3 qrk_shadeAllSpotLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                      vec3 normal, vec2 texCoords, float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_spotLightCount; i++) {
    result += qrk_shadeSpotLightBlinnPhong(material, qrk_spotLights[i], fragPos,
                                           normal, texCoords, ao);
  }
  return result;
}

/** Calculate shading from all active spot lights using deferred data. */
vec3 qrk_shadeAllSpotLightsDeferredBlinnPhong(vec3 albedo, vec3 specular,
                                              vec3 ambient, float shininess,
                                              vec3 fragPos, vec3 normal,
                                              float ao) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < qrk_spotLightCount; i++) {
    result += qrk_shadeSpotLightBlinnPhongDeferred(albedo, specular, ambient,
                                                   shininess, qrk_spotLights[i],
                                                   fragPos, normal, ao);
  }
  return result;
}

/** Calculate shading from all light sources, except emission textures. */
vec3 qrk_shadeAllLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                  vec3 normal, vec2 texCoords, float shadow,
                                  float ao) {
  vec3 directional = qrk_shadeAllDirectionalLightsBlinnPhong(
      material, fragPos, normal, texCoords, shadow, ao);
  vec3 point = qrk_shadeAllPointLightsBlinnPhong(material, fragPos, normal,
                                                 texCoords, ao);
  vec3 spot = qrk_shadeAllSpotLightsBlinnPhong(material, fragPos, normal,
                                               texCoords, ao);
  return directional + point + spot;
}

vec3 qrk_shadeAllLightsBlinnPhong(QrkMaterial material, vec3 fragPos,
                                  vec3 normal, vec2 texCoords) {
  return qrk_shadeAllLightsBlinnPhong(material, fragPos, normal, texCoords,
                                      /*shadow=*/0.0, /*ao=*/1.0);
}

/** Calculate shading from all light sources, except emission textures, using
 * deferred data. */
vec3 qrk_shadeAllLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                          vec3 ambient, float shininess,
                                          vec3 fragPos, vec3 normal,
                                          float shadow, float ao) {
  vec3 directional = qrk_shadeAllDirectionalLightsBlinnPhongDeferred(
      albedo, specular, ambient, shininess, fragPos, normal, shadow, ao);
  vec3 point = qrk_shadeAllPointLightsBlinnPhongDeferred(
      albedo, specular, ambient, shininess, fragPos, normal, ao);
  vec3 spot = qrk_shadeAllSpotLightsDeferredBlinnPhong(
      albedo, specular, ambient, shininess, fragPos, normal, ao);
  return directional + point + spot;
}

vec3 qrk_shadeAllLightsBlinnPhongDeferred(vec3 albedo, vec3 specular,
                                          vec3 ambient, float shininess,
                                          vec3 fragPos, vec3 normal) {
  return qrk_shadeAllLightsBlinnPhongDeferred(albedo, specular, ambient,
                                              shininess, fragPos, normal,
                                              /*shadow=*/0.0, /*ao=*/1.0);
}