#version 330 core
in vec2 texCoords;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

struct Attenuation {
  float constant;
  float linear;
  float quadratic;
};

#define NUM_DIFFUSE_TEXTURES 1
#define NUM_SPECULAR_TEXTURES 1
#define NUM_EMISSION_TEXTURES 1
struct Material {
  sampler2D diffuse[NUM_DIFFUSE_TEXTURES];
  sampler2D specular[NUM_SPECULAR_TEXTURES];
  sampler2D emission[NUM_EMISSION_TEXTURES];
  float shininess;

  Attenuation emissionAttenuation;
};

struct DirectionalLight {
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  Attenuation attenuation;
};

struct SpotLight {
  vec3 position;
  vec3 direction;
  float innerAngle;
  float outerAngle;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  Attenuation attenuation;
};

uniform float time;
uniform Material material;

#define NUM_DIRECTIONAL_LIGHTS 1
uniform DirectionalLight directionalLights[NUM_DIRECTIONAL_LIGHTS];

#define NUM_POINT_LIGHTS 1
uniform PointLight pointLights[NUM_POINT_LIGHTS];

#define NUM_SPOT_LIGHTS 1
uniform SpotLight spotLights[NUM_SPOT_LIGHTS];

/**
 * Calculate the Phong shading model with ambient, diffuse, and specular
 * components. Does not include attenuation.
 */
vec3 shadePhong(Material material, vec3 lightAmbient, vec3 lightDiffuse,
                vec3 lightSpecular, vec3 lightDir, vec3 viewDir, vec3 normal,
                vec2 texCoords, float intensity) {
  vec3 result = vec3(0.0);

  // Ambient and diffuse components.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  for (int i = 0; i < NUM_DIFFUSE_TEXTURES; i++) {
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
  for (int i = 0; i < NUM_SPECULAR_TEXTURES; i++) {
    vec3 specularMap = vec3(texture(material.specular[i], texCoords));
    result += (lightSpecular * (specularIntensity * specularMap)) * intensity;
  }

  return result;
}

/**
 * Calculate attenuation based on fragment distance from a light source.
 * Returns a multipler that can be used in shading.
 */
float calcAttenuation(Attenuation attenuation, float fragDist) {
  return 1.0 / (attenuation.constant + attenuation.linear * fragDist +
                attenuation.quadratic * (fragDist * fragDist));
}

vec3 shadeDirectionalLight(Material material, DirectionalLight light,
                           vec3 fragPos, vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(-light.direction);
  vec3 viewDir = normalize(-fragPos);

  return shadePhong(material, light.ambient, light.diffuse, light.specular,
                    lightDir, viewDir, normal, texCoords, /* intensity */ 1.0);
}

vec3 shadePointLight(Material material, PointLight light, vec3 fragPos,
                     vec3 normal, vec2 texCoords) {
  vec3 lightDir = normalize(light.position - fragPos);
  vec3 viewDir = normalize(-fragPos);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = calcAttenuation(light.attenuation, lightDist);

  vec3 result =
      shadePhong(material, light.ambient, light.diffuse, light.specular,
                 lightDir, viewDir, normal, texCoords, /* intensity */ 1.0);
  // Apply attenuation.
  return result * attenuation;
}

vec3 shadeSpotLight(Material material, SpotLight light, vec3 fragPos,
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
  float attenuation = calcAttenuation(light.attenuation, lightDist);

  vec3 result =
      shadePhong(material, light.ambient, light.diffuse, light.specular,
                 lightDir, viewDir, normal, texCoords, intensity);
  // Apply attenuation.
  return result * attenuation;
}

vec3 shadeEmission(Material material, vec3 fragPos, vec2 texCoords) {
  vec3 result = vec3(0.0);

  // Calculate emission attenuation towards camera.
  float fragDist = length(fragPos);
  float attenuation = calcAttenuation(material.emissionAttenuation, fragDist);

  // Emission component.
  for (int i = 0; i < NUM_EMISSION_TEXTURES; i++) {
    result += vec3(texture(material.emission[i], texCoords)) * attenuation;
  }

  // Special effect.
  // TODO: Remove.
  return (sin(time * 0.5) * 0.2 + 0.2) * result;
}

void main() {
  vec3 normal = normalize(fragNormal);

  vec3 result = vec3(0.0);

  // Shade with directional lights.
  for (int i = 0; i < NUM_DIRECTIONAL_LIGHTS; i++) {
    result += shadeDirectionalLight(material, directionalLights[i], fragPos,
                                    normal, texCoords);
  }

  // Shade with point lights.
  for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
    result +=
        shadePointLight(material, pointLights[i], fragPos, normal, texCoords);
  }

  // Shade with spot lights.
  for (int i = 0; i < NUM_SPOT_LIGHTS; i++) {
    result +=
        shadeSpotLight(material, spotLights[i], fragPos, normal, texCoords);
  }

  // Add emissions.
  result += shadeEmission(material, fragPos, texCoords);

  fragColor = vec4(result, 1.0);
}
