#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 viewProjection;


out VS_OUT {
	vec3 FragPos;
} vs_out;
uniform sampler2D highMap;

void main()
{
    vec3 pos;
    if (aPos.y > 0)
        pos = vec3(aPos.x, texture2DLod(highMap, aPos.xz,0).r, aPos.z);
    else
        pos = aPos;
//    vec3 pos = vec3(aPos.x, 1.0, aPos.z);
    vs_out.FragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = viewProjection * vec4(vs_out.FragPos, 1.0);
}       