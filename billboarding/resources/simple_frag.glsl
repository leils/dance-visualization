#version 330 core
in vec3 fragNor;
in vec3 fragPos;
out vec4 color;
in vec3 light_I;
in float fragColor;
uniform vec3 lightDir;

uniform sampler2D Texture0;
in vec2 vTexCoord;

uniform bool knee;

void main()
{
    float intensity;
	intensity = dot(lightDir ,normalize(fragNor));

    vec4 red = vec4(1,0,0,1);
    vec4 black = vec4(0,0,0,1);
    if (knee) {
        color =  texture(Texture0, vTexCoord);
        //color = black;
    } else {
       //vec3 normal = normalize(fragNor);
       //color = red * intensity;
       vec4 tempColor = vec4(fragColor, 0, 0, 1);
       //color = tempColor * intensity;
       color = tempColor;
    }

}
