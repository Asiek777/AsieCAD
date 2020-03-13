#version 330 core

layout (location = 0) in vec4 aPos;
layout(location = 1) in vec4 particlePos;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{


	TexCoord = aPos.zw;
	mat4 VP = projection * view;       
	vec3 CameraRight = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 CameraUp = vec3(view[0][1], view[1][1], view[2][1]);

	vec3 Pos = particlePos.xyz;  // The desired point for the billboard

	float size = 0.1;
	gl_Position = VP * vec4(Pos + CameraRight * aPos.x * size + CameraUp * aPos.y * size, 1.0);  

}