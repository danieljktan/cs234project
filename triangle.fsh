#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec3 _objectColor;
in vec3 _lightColor;
in vec3 _lightPos;
in vec3 _viewPos;
out vec4 FragColor;


void main()
{
  float ambientStrength = 0.1f;
  float specularStrength = 0.5f;
  vec3 ambient = ambientStrength * _lightColor;
  vec3 norm = normalize(Normal);
  vec3 lightDir = normalize(_lightPos);
  float diff = max(dot(norm,lightDir), 0.0f);
  vec3 diffuse = diff * _lightColor;
  vec3 viewDir = normalize(_viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, norm);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
  vec3 specular = specularStrength * spec * _lightColor;
  vec3 result = (diffuse+ambient+specular) * _objectColor;
  FragColor = vec4(result, 1.0f);
}

