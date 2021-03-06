// Renders ribbons in repeated textured form
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

    vec4 texcolor =  texture(Texture0, vTexCoord);
    if (texcolor.r > .5 && texcolor.g > .5 && texcolor.b > .5){
        discard;
    }
    color = texcolor;

}
