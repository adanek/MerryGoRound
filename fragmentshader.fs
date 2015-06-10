#version 330

uniform vec3 EyeDirection;
uniform vec3 Attenuation;
uniform float Ambient;

uniform int ShowAmbient;
uniform int ShowDiffuse;
uniform int ShowSpecular;

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

uniform mat4 ViewMatrix;
uniform int UseTexture;

in vec2 UVcoords;

uniform sampler2D myTextureSampler;

void main()
{

	int TRUE = 1;
	vec4 color;
	
	//color buffer
	if(UseTexture != TRUE){
		color = vColor;
	//texture
	}else{
		color = texture2D(myTextureSampler, UVcoords);
	}

    vec3 ambient = vec3(Ambient);
    vec3 v = normalize(EyeDirection);
    vec3 n  =normalize(vNormal);
    
    // Lightsource 1
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
	

    vec3 sl1 = lcol * diffuse * attenuation;
    vec3 rl1 = lcol * specular * Strength * attenuation;
   
   // Light source 2 
    lpos = L2_Position;
    lcol = L2_Color;

    lightDirection = lpos - vec3(vPosition);
    lightDistance = length(lightDirection);    
    lightDirection = lightDirection / lightDistance;

    // Calculate attenuation
    attenuation = 1.0 / (Attenuation.x + Attenuation.y * lightDistance + Attenuation.z * lightDistance * lightDistance);

    // Calculate halfvector
    h = normalize(lightDirection + v);   


	diffuse = max(0, dot(n, lightDirection));    
	specular = max(0, dot(n, h));	
    
	if(diffuse == 0.0){
        specular = 0.0;
    } else {
    	specular = pow(specular, Shininess);
    }	

    vec3 sl2 = lcol * diffuse * attenuation;
    vec3 rl2 = lcol * specular * attenuation;


    if(ShowAmbient != TRUE){
        ambient = vec3(0);
    }

    if(ShowDiffuse != TRUE){
        sl1 = vec3(0);
        sl2 = vec3(0);
    }

    if(ShowSpecular != TRUE){
        rl1 = vec3(0);
        rl2 = vec3(0);
    }

    vec3 rgb = min(color.rgb * (ambient + sl1 + sl2) + rl1 + rl2, vec3(1.0));
    FragColor = vec4(rgb, color.a);
}