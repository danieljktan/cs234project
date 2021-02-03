#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec3 _objectColor;
out vec3 _lightColor;
out vec3 _lightPos;
out vec3 _viewPos;

void main()
{
  gl_Position = projection*view*model*vec4(aPos, 1.0f);
  FragPos = vec3(model * vec4(aPos,1.0));
  _objectColor = objectColor;
  _lightColor = lightColor;
  _lightPos = lightPos;
  _viewPos = viewPos;
  Normal = aNormal;
}
