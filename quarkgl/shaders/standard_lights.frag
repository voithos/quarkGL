#ifndef QUARKGL_STANDARD_LIGHTS_FRAG_
#define QUARKGL_STANDARD_LIGHTS_FRAG_
#pragma qrk_include < lighting.frag >

#define NUM_DIRECTIONAL_LIGHTS 1
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];

#define NUM_POINT_LIGHTS 1
uniform PointLight pointLights[NUM_POINT_LIGHTS];

#define NUM_SPOT_LIGHTS 1
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

/** Calculate shading from all active directional lights. */
vec3 shadeAllDirectionalLights(Material material, vec3 fragPos, vec3 normal,
                               vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    result += shadeDirectionalLight(material, directionalLights[i], fragPos,
                                    normal, texCoords);
  }
  return result;
}

/** Calculate shading from all active point lights. */
vec3 shadeAllPointLights(Material material, vec3 fragPos, vec3 normal,
                         vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
    result +=
        shadePointLight(material, pointLights[i], fragPos, normal, texCoords);
  }
  return result;
}

/** Calculate shading from all active spot lights. */
vec3 shadeAllSpotLights(Material material, vec3 fragPos, vec3 normal,
                        vec2 texCoords) {
  vec3 result = vec3(0.0);
  for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
    result +=
        shadeSpotLight(material, spotLights[i], fragPos, normal, texCoords);
  }
  return result;
}

/** Calculate shading from all light sources, except emission textures. */
vec3 shadeAllLights(Material material, vec3 fragPos, vec3 normal,
                    vec2 texCoords) {
  vec3 directional =
      shadeAllDirectionalLights(material, fragPos, normal, texCoords);
  vec3 point = shadeAllPointLights(material, fragPos, normal, texCoords);
  vec3 spot = shadeAllSpotLights(material, fragPos, normal, texCoords);
  return directional + point + spot;
}

#endif
