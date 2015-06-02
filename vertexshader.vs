#version 330

uniform mat4 ProjectionMatrix; 
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

uniform vec4 FixedColor;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec3 VertexNormal;

out vec4 vColor;
out vec3 vNormal;

void main()
{
   vColor = FixedColor;

   vNormal = normalize(ModelMatrix * VertexNormal);
   gl_Position = ProjectionMatrix*ViewMatrix*ModelMatrix*vec4(Position.x, Position.y, Position.z, 1.0);
 
}
