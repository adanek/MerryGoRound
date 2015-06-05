#version 330

uniform vec3 EyeDirection;
uniform vec3 Attenuation;
uniform float ambient;

uniform int SHOW_AMBIENT;
uniform int SHOW_DIFFUSE;
uniform int SHOW_SPECULAR;

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
   
     vec3 scatteredLight = vec3(0);
     vec3 reflectedLight = vec3(0);

    if(SHOW_AMBIENT == 1){
        scatteredLight = scatteredLight + vec3(ambient);
    }

    // Lightsource 1

    vec3 sL = vec3(0);
    vec3 r1 = vec3(0);

    vec3 v = normalize(EyeDirection);
    vec3 n  =normalize(vNormal);

    // Set LS1
    vec3 lpos = L1_Position;
    vec3 lcol = L1_Color;
    

    vec3 lightDirection = lpos - vec3(vPosition);
    float lightDistance = length(lightDirection);    
    lightDirection = lightDirection / lightDistance;

    // Calculate attenuation
     float attenuation = 1.0 / (Attenuation.x + Attenuation.y * lightDistance + Attenuation.z * lightDistance * lightDistance);

    // Calculate halfvector
    vec3 h = normalize(lightDirection + v);   


	float diffuse = max(0, dot(n, lightDirection));    
	float specular = max(0, dot(n, h));	
    
	if(diffuse == 0.0){
        specular = 0.0;
    } else {
    	specular = pow(specular, Shininess);
    }	
	

    scatteredLight = vec3(ambient) + lcol * diffuse * attenuation;
    reflectedLight = lcol * specular * Strength * attenuation;
    


    vec3 rgb = min(vColor.rgb * scatteredLight + reflectedLight, vec3(1.0));
    FragColor = vec4(rgb, vColor.a);
}