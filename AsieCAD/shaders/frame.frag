#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D frame0;
uniform sampler2D frame1;
uniform vec3 leftFilter;
uniform vec3 rightFilter;

void main()
{
    vec3 col0 = texture(frame0, TexCoords).rgb;
    vec3 col1 = texture(frame1, TexCoords).rgb;
    vec3 col = col0 * leftFilter + col1 * rightFilter;
    FragColor = vec4(col, 1.0);
} 