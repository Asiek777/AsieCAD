#version 330 core
layout (location = 0) in float aPos;


void main()
{
    gl_Position = vec4(aPos, 0, 0, 1.0);
}