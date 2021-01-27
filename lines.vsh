#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
  gl_Position = model*projection*view*vec4(aPos, 1.0f);
  //gl_Position = vec4(aPos, 1.0f);
}
