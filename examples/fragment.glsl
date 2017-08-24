#version 330 core
in vec3 fragPos;
in vec3 fragNormal;
in vec3 lightPos;

out vec4 fragColor;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform Light light;

void main() {
  vec3 normal = normalize(fragNormal);
  vec3 lightDir = normalize(lightPos - fragPos);

  // Ambient.
  vec3 ambient = light.ambient * material.ambient;

  // Diffuse.
  float diffuseIntensity = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * material.diffuse);

  // Specular.
  vec3 viewDir = normalize(-fragPos);
  vec3 reflectDir = reflect(-lightDir, normal);
  float specularIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * material.specular);

  vec3 result = ambient + diffuse + specular;
  fragColor = vec4(result, 1.0);
}
