#version 330

in vec4 vColor;
in vec3 vNormal;
in vec4 vPosition;
uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 HalfVector;
uniform float Shininess;
uniform float Strength;
uniform float ambient;

out vec4 FragColor;

void main()
{

	vec3 h = normalize(LightDirection - vec3(vPosition));

	float diffuse = max(0.0, dot(vNormal, LightDirection));
	float specular = max(0.0, dot(vNormal, h));
	
	if(diffuse == 0.0)
		specular = 0.0;
	else
		specular = pow(specular, Shininess);
	
    vec3 scatteredLight = vec3(ambient) + LightColor * diffuse;
    vec3 reflectedLight = LightColor * specular * Strength;
    
    vec3 rgb = min(vColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
    FragColor = vec4(rgb, vColor.a);
}