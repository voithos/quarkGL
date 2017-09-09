#version 330 core
in vec2 texCoords;
in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct Attenuation {
  float constant;
  float linear;
  float quadratic;
};

struct Light {
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  Attenuation attenuation;
};

uniform Material material;
uniform Light light;
uniform float time;

void main() {
  vec3 normal = normalize(fragNormal);

  // Calculate attenuation from point light source.
  float lightDist = length(light.position - fragPos);
  float attenuation = 1.0 / (
      light.attenuation.constant +
      light.attenuation.linear * lightDist +
      light.attenuation.quadratic * (lightDist * lightDist));

  // Calculate attenuation from camera.
  float fragDist = length(fragPos);
  float fragAttenuation = 1.0 / (
      light.attenuation.constant +
      light.attenuation.linear * fragDist +
      light.attenuation.quadratic * (fragDist * fragDist));

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
  float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * specularMap) * attenuation;

  // Emission.
  vec3 emission = vec3(texture(material.emission, texCoords)) * fragAttenuation;
  emission = (sin(time) * 0.5 + 0.5) * 2.0 * emission;

  vec3 result = ambient + diffuse + specular + emission;
  fragColor = vec4(result, 1.0);
}
