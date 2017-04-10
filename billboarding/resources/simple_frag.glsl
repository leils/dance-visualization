#version 330 core
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;
in vec3 light_I;

uniform bool knee;

void main()
{
    float intensity;
	//intensity = dot(lightDir,normalize(fragNor));


    vec4 red = vec4(1,0,0,1);
    vec4 black = vec4(0,0,0,1);
    if (knee) {
        color =  black;
    } else {
       vec3 normal = normalize(fragNor);
       //color = red * intensity;
       //color = red;
       color = vec4(normal, 1);

    }

}
