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

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;

  Attenuation emissionAttenuation;
};

struct PointLight {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  Attenuation attenuation;
};

uniform float time;
uniform Material material;

#define NUM_POINT_LIGHTS 1
uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 shadePointLight(PointLight light, Material material, vec3 fragPos,
                     vec3 normal, vec2 texCoords) {
  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation =
      1.0 / (light.attenuation.constant + light.attenuation.linear * lightDist +
             light.attenuation.quadratic * (lightDist * lightDist));

  vec3 lightDir = normalize(light.position - fragPos);
  vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));

  // Ambient.
  vec3 ambient = light.ambient * diffuseMap * attenuation;

  // Diffuse.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * diffuseMap) * attenuation;

  // Specular.
  vec3 specularMap = vec3(texture(material.specular, texCoords));
  vec3 viewDir = normalize(-fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularIntensity =
      pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular =
      light.specular * (specularIntensity * specularMap) * attenuation;

  return ambient + diffuse + specular;
}

vec3 shadeEmission(Material material, vec3 fragPos, vec2 texCoords) {
  // Calculate attenuation towards camera.
  float fragDist = length(fragPos);
  float attenuation =
      1.0 / (material.emissionAttenuation.constant +
             material.emissionAttenuation.linear * fragDist +
             material.emissionAttenuation.quadratic * (fragDist * fragDist));

  // Emission.
  vec3 emission = vec3(texture(material.emission, texCoords)) * attenuation;

  // Special effect.
  // TODO: Remove.
  return (sin(time * 0.5) * 0.2 + 0.2) * emission;
}

void main() {
  vec3 normal = normalize(fragNormal);

  vec3 result = vec3(0.0);

  // Shade with point lights.
  for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
    result +=
        shadePointLight(pointLights[i], material, fragPos, normal, texCoords);
  }

  // Add emissions.
  result += shadeEmission(material, fragPos, texCoords);

  fragColor = vec4(result, 1.0);
}
