#pragma once

#pragma qrk_include < constants.glsl>
#pragma qrk_include < lighting.frag>

/**
 * Returns the GGX microfacet distribution function.
 */
float qrk_distributionGGX(float NdotH, float a) {
  float a2 = a * a;
  float f = (NdotH * a2 - NdotH) * NdotH + 1.0;
  return a2 / (PI * f * f);
}

/**
 * Returns the geometric visibility term based on the Smith-GGX approximation.
 * This is the shadowing-masking term used in Cook-Torrance microfacet models.
 * This formulation cancels out the standard BRDF denominator (4 * NdotV *
 * NdotL).
 * https://google.github.io/filament/Filament.html#materialsystem/specularbrdf/geometricshadowing(specularg)
 */
float qrk_visibilitySmithGGXCorrelated(float NdotV, float NdotL, float a) {
  float a2 = a * a;
  float GGXV = NdotL * sqrt((NdotV - NdotV * a2) * NdotV + a2);
  float GGXL = NdotV * sqrt((NdotL - NdotL * a2) * NdotL + a2);
  return 0.5 / (GGXV + GGXL);
}

/**
 * Returns Schlick's approximation of the Fresnel factor.
 *   F(w_i, h) = F0 + (1 - F0) * (1 - (w_i • h))^5
 * where F0 is the surface reflectance at zero incidence.
 * https://en.wikipedia.org/wiki/Schlick%27s_approximation
 */
vec3 qrk_fresnelSchlick(float LdotH, vec3 F0) {
  return F0 + (vec3(1.0) - F0) * pow(1.0 - LdotH, 5.0);
}

/**
 * A variant of Schlick's approximation that takes a roughness "fudge factor".
 * Meant to be used when computing the Fresnel factor on a prefiltered map (i.e.
 * a color that is actually sampled from many directions, not just 1).
 * https://seblagarde.wordpress.com/2011/08/17/hello-world/
 */
vec3 qrk_fresnelSchlickRoughness(float LdotV, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - LdotV, 5.0);
}

// TODO: Move to an IBL shader file.

vec3 qrk_samplePrefilteredEnvMap(vec3 viewDir_worldSpace,
                                 vec3 normal_worldSpace, float roughness,
                                 samplerCube prefilteredEnvMap,
                                 float prefilteredEnvMapMaxLOD) {
  vec3 reflectionDir_worldSpace =
      reflect(-viewDir_worldSpace, normal_worldSpace);
  float mipLevel = roughness * prefilteredEnvMapMaxLOD;
  return textureLod(prefilteredEnvMap, reflectionDir_worldSpace, mipLevel).rgb;
}

vec2 qrk_sampleBrdfLUT(vec3 viewDir_worldSpace, vec3 normal_worldSpace,
                       float roughness, sampler2D brdfLUT) {
  float NdotV = clamp(dot(normal_worldSpace, viewDir_worldSpace), 0.0, 1.0);
  return texture(brdfLUT, vec2(NdotV, roughness)).rg;
}

/** Calculate the ambient IBL shading component. */
vec3 qrk_shadeAmbientIBLDeferred(vec3 albedo, vec3 irradiance,
                                 vec3 prefilteredEnvColor, vec2 envBRDF,
                                 float roughness, float metallic, float ao,
                                 vec3 viewDir, vec3 normal) {
  // Compute reflectance at normal incidence.
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // Use the roughness-aware fresnel function to compute the specular component.
  vec3 F = qrk_fresnelSchlickRoughness(clamp(dot(normal, viewDir), 0.0, 1.0),
                                       F0, roughness);
  vec3 kS = F;
  vec3 kD = 1.0 - kS;
  kD *= 1.0 - metallic;

  vec3 diffuse = kD * irradiance * albedo;
  // No need to multiply by kS, since F is already here.
  vec3 specular = prefilteredEnvColor * (F * envBRDF.x + envBRDF.y);

  return (diffuse + specular) * ao;
}

/**
 * Calculate shading for ambient IBL component based on the given
 * material.
 */
vec3 qrk_shadeAmbientIrradiance(QrkMaterial material, vec2 texCoords,
                                vec3 irradiance, vec3 prefilteredEnvColor,
                                vec2 envBRDF, vec3 viewDir, vec3 normal) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  float roughness = qrk_extractRoughness(material, texCoords);
  float metallic = qrk_extractMetallic(material, texCoords);
  float ao = qrk_extractAmbientOcclusion(material, texCoords);
  return qrk_shadeAmbientIBLDeferred(albedo, irradiance, prefilteredEnvColor,
                                     envBRDF, roughness, metallic, ao, viewDir,
                                     normal);
}

/**
 * Calculate the deferred Cook-Torrance shading model with diffuse, and
 * specular components, along with direct material colors. Does not include
 * attenuation.
 */
vec3 qrk_shadeCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                      float metallic, vec3 lightDiffuse,
                                      vec3 lightSpecular, vec3 lightDir,
                                      vec3 viewDir, vec3 normal) {
  vec3 halfVector = normalize(lightDir + viewDir);

  // Compute reflectance at normal incidence. For dielectrics (like plastic),
  // we use a typical F0 of 0.04, whereas metals tend to be more reflective
  // and also chromatic (they tint the reflection with their albedo).
  // TODO: This can be extracted as a material parameter for more control.
  vec3 F0 = vec3(0.04);
  F0 = mix(F0, albedo, metallic);

  // Incoming roughness should be "perceptually linear", so we remap it.
  // We also clamp the roughness to a small value so as to avoid INF in some of
  // the calculations.
  float a = max(roughness * roughness, 0.002025);

  // TODO: Why abs() and not clamp?
  float NdotV = max(dot(normal, viewDir), 1e-4);
  //   float NdotV = clamp(dot(normal, viewDir), 0.0, 1.0);
  float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);
  float NdotH = clamp(dot(normal, halfVector), 0.0, 1.0);
  float LdotH = clamp(dot(lightDir, halfVector), 0.0, 1.0);

  float D = qrk_distributionGGX(NdotH, a);
  vec3 F = qrk_fresnelSchlick(LdotH, F0);
  float V = qrk_visibilitySmithGGXCorrelated(NdotV, NdotL, a);

  // Specular BRDF.
  vec3 specular = (D * V) * F;

  // Diffuse BRDF (Lambertian).
  // We remap the albedo for metals since they only have a specular component
  // and emit no diffuse light.
  vec3 diffuseColor = (1.0 - metallic) * albedo;
  vec3 diffuse = diffuseColor / PI;

  // Calculate radiance.
  return (diffuse * lightDiffuse + specular * lightSpecular) * NdotL;
}

/**
 * Calculate the Cook-Torrance shading model with diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 qrk_shadeCookTorranceGGX(QrkMaterial material, vec3 lightDiffuse,
                              vec3 lightSpecular, vec3 lightDir, vec3 viewDir,
                              vec3 normal, vec2 texCoords) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  float roughness = qrk_extractRoughness(material, texCoords);
  float metallic = qrk_extractMetallic(material, texCoords);
  // Use the deferred calculations, but do it directly, so it's not deferred.
  // :)
  return qrk_shadeCookTorranceGGXDeferred(albedo, roughness, metallic,
                                          lightDiffuse, lightSpecular, lightDir,
                                          viewDir, normal);
}

/** Calculate shading for a directional light source using deferred data. */
vec3 qrk_shadeDirectionalLightCookTorranceGGXDeferred(
    vec3 albedo, float roughness, float metallic, QrkDirectionalLight light,
    vec3 fragPos, vec3 normal, float shadow) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  float shadowMultiplier = 1.0 - shadow;

  vec3 result = qrk_shadeCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                 light.diffuse, light.specular,
                                                 lightDir, viewDir, normal);
  // Apply shadowing, if any.
  return result * shadowMultiplier;
}

/** Calculate shading for a directional light source. */
vec3 qrk_shadeDirectionalLightCookTorranceGGX(QrkMaterial material,
                                              QrkDirectionalLight light,
                                              vec3 fragPos, vec3 normal,
                                              vec2 texCoords, float shadow) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  float roughness = qrk_extractRoughness(material, texCoords);
  float metallic = qrk_extractMetallic(material, texCoords);
  return qrk_shadeDirectionalLightCookTorranceGGXDeferred(
      albedo, roughness, metallic, light, fragPos, normal, shadow);
}

/** Calculate shading for a point light source using deferred data. */
vec3 qrk_shadePointLightCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                                float metallic,
                                                QrkPointLight light,
                                                vec3 fragPos, vec3 normal) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  vec3 result = qrk_shadeCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                 light.diffuse, light.specular,
                                                 lightDir, viewDir, normal);
  // Apply attenuation.
  return result * attenuation;
}

/** Calculate shading for a point light source. */
vec3 qrk_shadePointLightCookTorranceGGX(QrkMaterial material,
                                        QrkPointLight light, vec3 fragPos,
                                        vec3 normal, vec2 texCoords) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  float roughness = qrk_extractRoughness(material, texCoords);
  float metallic = qrk_extractMetallic(material, texCoords);
  return qrk_shadePointLightCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                    light, fragPos, normal);
}

/** Calculate shading for a spot light source using deferred data. */
vec3 qrk_shadeSpotLightCookTorranceGGXDeferred(vec3 albedo, float roughness,
                                               float metallic,
                                               QrkSpotLight light, vec3 fragPos,
                                               vec3 normal) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = qrk_calcAttenuation(light.attenuation, lightDist);

  float spotlightIntensity = qrk_calcSpotLightIntensity(light, lightDir);

  vec3 result = qrk_shadeCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                 light.diffuse, light.specular,
                                                 lightDir, viewDir, normal);
  // Apply attenuation and intensity.
  return result * attenuation * spotlightIntensity;
}

/** Calculate shading for a spot light source. */
vec3 qrk_shadeSpotLightCookTorranceGGX(QrkMaterial material, QrkSpotLight light,
                                       vec3 fragPos, vec3 normal,
                                       vec2 texCoords) {
  vec3 albedo = qrk_extractAlbedo(material, texCoords);
  float roughness = qrk_extractRoughness(material, texCoords);
  float metallic = qrk_extractMetallic(material, texCoords);
  return qrk_shadeSpotLightCookTorranceGGXDeferred(albedo, roughness, metallic,
                                                   light, fragPos, normal);
}