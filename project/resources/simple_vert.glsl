#version  330 core
//Gauroud Shading
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in float vertColor;
layout(location = 3) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
out vec3 fragNor;
out vec3 fragPos;
out float fragColor;
out vec2 vTexCoord;

uniform bool knee;


void main()
{
   fragPos = (vertPos * M).xyz;

   gl_Position = P * V * M * vertPos;
   fragNor = (M * vec4(vertNor, 0.0)).xyz;

   fragColor = vertColor;

   vTexCoord = vertTex;
}
