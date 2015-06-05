#version 330

uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 LightPosition;
uniform vec3 EyeDirection;
uniform float Shininess;
uniform float Strength;
uniform float ambient;

in vec4 vColor;
in vec3 vNormal;
in vec4 vPosition;

out vec4 FragColor;

void main()
{
    
    vec3 lightDirection = LightPosition - vec3(vPosition);
    float lightDistance = length(lightDirection);

    // Normalize lightdirection
    lightDirection = lightDirection / lightDistance;

    vec3 l = normalize(LightDirection);
	vec3 v = normalize(EyeDirection);
    vec3 h = normalize(lightDirection + v);

	float diffuse = max(0.0, dot(vNormal, l));    
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