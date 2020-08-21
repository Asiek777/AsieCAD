#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 trimRange;
out vec3 coords;

uniform mat4 model;
uniform mat4 viewProjection;
uniform bool isBig;

void main()
{
    vec3 FragPos = vec3(model * vec4(aPos, 1.0));
    coords = vec3(texCoord[1], trimRange.zw);
    if (isBig) {
        coords = vec3(texCoord[0], trimRange.xy);
    }
    gl_Position = viewProjection * vec4(FragPos, 1.0);
}