#version 450 core
in vec2 texCoords;
out vec4 FragColor;
uniform vec3 color;
uniform bool reverseTrimming;
uniform bool isTrimmed;

uniform sampler2D tex;

void main()
{
    if (isTrimmed) {
        float alfa = texture(tex, texCoords.yx).r;
        if (reverseTrimming)
            alfa = 1 - alfa;
        FragColor = vec4(color, alfa);
    }
    else
        FragColor = vec4(color, 1.0);
} 