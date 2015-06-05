#version 330

uniform vec3 EyeDirection;
uniform vec3 Attenuation;
uniform float ambient;

uniform vec3 L1_Color;
uniform vec3 L1_Position;

uniform vec3 L2_Color;
uniform vec3 L2_Position;

uniform float Shininess;
uniform float Strength;

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

    float attenuation = 1.0 / (Attenuation.x + Attenuation.y * lightDistance + Attenuation.z * lightDistance * lightDistance);

	float diffuse = max(0.0, dot(vNormal, l));    
	float specular = max(0.0, dot(vNormal, h));
	
	if(diffuse == 0.0)
		specular = 0.0;
	else
		specular = pow(specular, Shininess);
	
    vec3 scatteredLight = vec3(ambient) + LightColor * diffuse * attenuation;
    vec3 reflectedLight = LightColor * specular * Strength * attenuation;
    
    vec3 rgb = min(vColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
    FragColor = vec4(rgb, vColor.a);
}