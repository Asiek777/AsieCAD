#version 330 core
layout (location = 0) in vec3 aPos;


uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = viewProjection * vec4(FragPos, 1.0);
}