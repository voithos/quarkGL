#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(LightPos - FragPos);

  // Ambient.
  float ambientStrength = 0.1;
  vec3 ambient = lightColor * ambientStrength;

  // Diffuse.
  float diff = max(dot(norm, lightDir), 0.0);
  vec3 diffuse = diff * lightColor;

  // Specular.
  float specularStrength = 0.5;
  vec3 viewDir = normalize(-FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * lightColor;

  vec3 result = (ambient + diffuse + specular) * objectColor;
  FragColor = vec4(result, 1.0);
}
