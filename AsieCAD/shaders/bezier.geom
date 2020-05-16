#version 330

layout(lines_adjacency) in;
layout(line_strip, max_vertices = 512) out;


vec4 toBezier3(float delta, int i, vec4 P0, vec4 P1, vec4 P2, vec4 P3)
{
    float t = delta * float(i);
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return (P0 * one_minus_t2 * one_minus_t + P1 * 3.0 * t * one_minus_t2 + 
        P2 * 3.0 * t2 * one_minus_t + P3 * t2 * t);
}
vec4 toBezier2(float delta, int i, vec4 P0, vec4 P1, vec4 P2)
{
    float t = delta * float(i);
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return (P0 * one_minus_t2  + P1 * 2.0 * t * one_minus_t + P2 * t2 );
}

void main(void)
{

    vec4 B[4];
    B[0] = gl_in[0].gl_Position;
    B[1] = gl_in[1].gl_Position;
    B[2] = gl_in[2].gl_Position;
    B[3] = gl_in[3].gl_Position;

    if(B[1] == B[2]) {
        gl_Position = B[0];
        EmitVertex();
        gl_Position = B[1];
        EmitVertex();
    } else if(B[2] == B[3]) {
        float dist = distance(B[0].xy, B[1].xy) + distance(B[1].xy, B[2].xy);
        int steps = min(int(dist * 10), 511);
        float delta = 1.0 / float(steps);
        for (int i=0; i<=steps; ++i){
            gl_Position = toBezier2(delta, i, B[0], B[1], B[2]);
            EmitVertex();
        }
    } else {
        float dist = distance(B[0].xy / B[0].w, B[1].xy / B[1].w) + 
            distance(B[1].xy / B[1].w, B[2].xy / B[2].w) + 
            distance(B[2].xy / B[2].w, B[3].xy / B[3].w);
        int steps = min(int(dist * 30), 511);
        float delta = 1.0 / float(steps);
        for (int i=0; i<=steps; ++i) {
            gl_Position = toBezier3(delta, i, B[0], B[1], B[2], B[3]);
            EmitVertex();
        }
    }  
    EndPrimitive();
}