#version 450
layout(points) in;
layout(line_strip, max_vertices = 512) out;

uniform vec3 p[20];
uniform bool isForward;
uniform mat4 viewProjection;


vec4 toBernstein3(float t)
{
    float t2 = t * t;
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    return vec4(one_minus_t2 * one_minus_t, 3.0 * t * one_minus_t2, 3.0 * t2 * one_minus_t, t2 * t);
}

void main(void)
{
    float x = gl_in[0].gl_Position.x, u, v;
    vec4 uCoord, vCoord;// = toBernstein3(u);
    if (isForward) {
        v = x;   
        vCoord = toBernstein3(x);
    }
    else {
        u = x;
        uCoord = toBernstein3(x);
    }

    vec3 mat[4][4];
    mat[0][0] = p[0];
    mat[0][1] = p[2];
    mat[0][2] = p[6];
    mat[0][3] = p[5];
    mat[1][3] = p[7];
    mat[2][3] = p[11];
    mat[3][3] = p[10];
    mat[3][2] = p[12];
    mat[3][1] = p[16];
    mat[3][0] = p[15];
    mat[2][0] = p[17];
    mat[1][0] = p[1];
    int steps = 100;
    float delta = 1.0 / float(steps);
    for (int i = 0; i <= steps; ++i) {
        if (isForward) {
            u = i * delta;
            uCoord = toBernstein3(u);
        }
        else {
            v = i * delta;
            vCoord = toBernstein3(v);
        }
        if(u != 0 && u != 1 && v != 0 && v != 1) {
            mat[1][1] = (u * p[4] + v * p[3]) / (u + v);
            mat[1][2] = ((1 - u) * p[8] + v * p[9]) / (1 - u + v);
            mat[2][1] = (u * p[18] + (1 - v) * p[19]) / (1 + u - v);
            mat[2][2] = ((1 - u) * p[14] + (1 - v) * p[13]) / (2 - u - v);
        }
        vec4 Pos = vec4(0, 0, 0, 1);
        for (int j = 0; j < 4; j++)
            for(int k = 0; k < 4; k++){
                Pos.xyz += mat[j][k] * uCoord[k] * vCoord[j];
            }
        gl_Position = viewProjection * Pos;
        EmitVertex();
    }

}