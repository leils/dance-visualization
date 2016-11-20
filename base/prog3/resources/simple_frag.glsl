#version 330 core 
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;
in vec3 Lpt;
in vec3 color_vec;
in vec3 light_I;
   
uniform int color_setting;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;

void main()
{
   vec3 normal = normalize(fragNor);

   //Gauroud Shading 
   if (color_setting == 1){
      color = vec4(color_vec, 1.0);
   }

   else if (color_setting == 2) {
      //temp for Phong shading 
      //color = vec4(1, 0, 0, 1);
      vec3 l = normalize(Lpt - fragPos);
      vec3 v = normalize(fragPos);

      vec3 color_dif;
      color_dif = MatDif * max(dot(l, normal), 0) * light_I;

      vec3 color_spec = vec3(0, 0, 0);
      vec3 H = normalize((v + l) / 2);
      color_spec = MatSpec * pow(max(dot(v, H), 0), shine) * light_I;

      color = vec4(color_dif + color_spec + MatAmb, 1.0);
   }

   //map normal to color 
   else {
      vec3 Ncolor = 0.5*normal + 0.5;
      color = vec4(Ncolor, 1.0); 
   }

}
