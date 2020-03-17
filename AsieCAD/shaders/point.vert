#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform vec3 position;
uniform mat4 viewProjection;

void main()
{
    gl_Position = viewProjection * vec4(position, 1.0);
}