#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 model;
uniform mat4 viewProjection;


out VS_OUT {
	vec3 FragPos;
	vec2 texCoords;
} vs_out;
uniform sampler2D tex;

void main()
{
    vec3 pos = vec3(aPos.x, texture2DLod(tex, aTexCoord,0).r, aPos.z);
//    vec3 pos = vec3(aPos.x, 1.0, aPos.z);
    vs_out.FragPos = vec3(model * vec4(pos, 1.0));
    vs_out.texCoords = aTexCoord;
    gl_Position = viewProjection * vec4(vs_out.FragPos, 1.0);
}       