#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frame0;
uniform sampler2D frame1;

void main()
{
    vec3 col0 = texture(frame0, TexCoords).rgb;
    vec3 col1 = texture(frame1, TexCoords).rgb;
    FragColor = vec4(col0 + col1, 1.0);
} 