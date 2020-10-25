#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 viewProjection;

out VS_OUT {
	vec3 FragPos;
	vec2 texCoords;
} vs_out;


void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.texCoords = vec2(0,0);
    gl_Position = viewProjection * vec4(vs_out.FragPos, 1.0);
}       