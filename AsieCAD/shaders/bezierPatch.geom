#version 330

layout(points) in;
layout(line_strip, max_vertices = 256) out;
out vec3 coords;

uniform mat4 Knots[3];
uniform bool isForward;
uniform vec2 coordsRange;
uniform mat4 viewProjection;


vec4 toBernstein3(float t)
{
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return vec4(one_minus_t2 * one_minus_t, 3.0 * t * one_minus_t2, 
        3.0 * t2 * one_minus_t, t2 * t);
}

vec4 toBezier3(float delta, int i, vec4 P0, vec4 P1, vec4 P2, vec4 P3)
{
    float t = delta * float(i);
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return (P0 * one_minus_t2 * one_minus_t + P1 * 3.0 * t * one_minus_t2 + 
        P2 * 3.0 * t2 * one_minus_t + P3 * t2 * t);
}

void main(void)
{
    float u = gl_in[0].gl_Position.x;
    vec4 uCoord = toBernstein3(u);
    mat4 knots[3];
    if (isForward)
        for(int i=0;i<3;i++)
            knots[i] = Knots[i];            
    else
        for(int i=0;i<3;i++)
            knots[i] = transpose(Knots[i]);

    vec4 xCoord = knots[0] * uCoord;
    vec4 yCoord = knots[1] * uCoord;
    vec4 zCoord = knots[2] * uCoord;
    vec4 B[4];
    B[0] = viewProjection * vec4(xCoord.x, yCoord.x, zCoord.x, 1);
    B[1] = viewProjection * vec4(xCoord.y, yCoord.y, zCoord.y, 1);
    B[2] = viewProjection * vec4(xCoord.z, yCoord.z, zCoord.z, 1);
    B[3] = viewProjection * vec4(xCoord.w, yCoord.w, zCoord.w, 1);

    float dist = distance(B[0].xy / B[0].w, B[1].xy / B[1].w) + 
        distance(B[1].xy / B[1].w, B[2].xy / B[2].w) + 
        distance(B[2].xy / B[2].w, B[3].xy / B[3].w) + 0.04;

    int steps = min(int(dist * 30), 255);
    float delta = 1.0 / float(steps);
    for (int i=0; i<=steps; ++i){
        gl_Position = toBezier3(delta, i, B[0], B[1], B[2], B[3]);
        coords = vec3(delta * i * (coordsRange[1] - coordsRange[0]) + coordsRange[0],
            gl_in[0].gl_Position.yz);
        EmitVertex();
    }
}