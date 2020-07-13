#version 330 core
in vec3 coords;
out vec4 FragColor;
uniform vec3 color;

void main()
{
    if (coords.x > coords.y && coords.x < coords.z)
        discard;
    FragColor = vec4(color, 1.0);
} 