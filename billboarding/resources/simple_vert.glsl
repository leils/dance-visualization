#version  330 core
//Gauroud Shading
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec3 fragNor;
out vec3 fragPos;
out vec3 Lpt;
out vec3 light_I;
out vec3 color_vec;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

uniform int light_x;

void main()
{
   Lpt = vec3(light_x, 1, 1);
   light_I = vec3(1, 1, 1);
   fragPos = (vertPos * M).xyz;

   gl_Position = P * V * M * vertPos;
   fragNor = (M * vec4(vertNor, 0.0)).xyz;

   color_vec = vec3(0, 0, 0);

}
