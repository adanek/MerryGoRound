#version 330

//uniform vec4 Ambient;

in vec4 vColor;
uniform float ambient;

out vec4 FragColor;

void main()
{
    vec3 scatteredLight = vec3(ambient);
    vec3 rgb = min(vColor.rgb * scatteredLight, vec3(1.0));
    FragColor = vec4(rgb, vColor.a);
}