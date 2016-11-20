#version  330 core
//Gauroud Shading
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 MV;
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
uniform int color_setting;

void main()
{
   Lpt = vec3(light_x, 1, 1);
   light_I = vec3(1, 1, 1);
   fragPos = (vertPos * MV).xyz;

   gl_Position = P * MV * vertPos;
   fragNor = (MV * vec4(vertNor, 0.0)).xyz;

   if (color_setting == 1) {
      //Gauroud Shading
      vec3 normal = normalize(fragNor);
      vec3 l = normalize(Lpt - fragPos);
      vec3 v = normalize(fragPos);

      vec3 color_dif;
      color_dif = MatDif * max(dot(l, normal), 0) * light_I;

      vec3 color_spec = vec3(0, 0, 0);
      vec3 H = normalize((v + l) / 2);
      color_spec = MatSpec * pow(max(dot(v, H), 0), shine) * light_I;

      color_vec = color_dif + color_spec + MatAmb;
   }

   else {
      color_vec = vec3(0, 0, 0);
   }

}
