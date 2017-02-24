#version 330 core
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;
in vec3 Lpt;
in vec3 color_vec;
in vec3 light_I;

uniform bool knee;

void main()
{
    if (knee) {
        color = vec4(0, 0, 0, 1);
    } else {
       //vec3 normal = normalize(fragNor);
       color = vec4(1, 0, 0, 1);

    }

}
