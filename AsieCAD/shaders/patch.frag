#version 450 core
in vec3 coords;
out vec4 FragColor;
uniform vec3 color;
uniform bool reverseTrimming;

void main()
{
    if ((coords.x > coords.y && coords.x < coords.z) ^^ reverseTrimming)
        discard;
    FragColor = vec4(color, 1.0);
} 