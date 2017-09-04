#version 330 core
in vec2 texCoords;
in vec3 fragPos;
in vec3 fragNormal;
in vec3 lightPos;

out vec4 fragColor;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

struct Light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform Light light;
uniform float time;

void main() {
  vec3 normal = normalize(fragNormal);
  vec3 lightDir = normalize(lightPos - fragPos);

  vec3 diffuseMap = vec3(texture(material.diffuse, texCoords));

  // Ambient.
  vec3 ambient = light.ambient * diffuseMap;

  // Diffuse.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * diffuseMap);

  // Specular.
  vec3 specularMap = vec3(texture(material.specular, texCoords));
  vec3 viewDir = normalize(-fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * specularMap);

  // Emission.
  vec3 emission = (sin(time) * 0.5 + 0.5) * 2.0 * vec3(texture(material.emission, texCoords));

  vec3 result = ambient + diffuse + specular + emission;
  fragColor = vec4(result, 1.0);
}
