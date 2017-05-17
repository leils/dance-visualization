#version  330 core
//Gauroud Shading
layout(location = 0) in vec4 vertPos;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec3 fragPos;

void main()
{
   fragPos = (vertPos * M).xyz;

   gl_Position = P * V * M * vertPos;
}
