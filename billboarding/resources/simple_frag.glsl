#version 330 core
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;
in vec3 Lpt;
in vec3 color_vec;
in vec3 light_I;

uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform float shine;
uniform bool knee;

void main()
{
   vec3 normal = normalize(fragNor);

      //temp for Phong shading
      //color = vec4(1, 0, 0, 1);
      vec3 l = normalize(Lpt - fragPos);
      vec3 v = normalize(fragPos);

      vec3 color_dif;
      color_dif = MatDif * max(dot(l, normal), 0) * light_I;

      vec3 color_spec = vec3(0, 0, 0);
      vec3 H = normalize((v + l) / 2);
      color_spec = MatSpec * pow(max(dot(v, H), 0), shine) * light_I;

      if (knee) {
        color = vec4(0, 0, 0, 1);

      } else {
        color = vec4(color_dif + color_spec + MatAmb, 1.0);

      }

}
