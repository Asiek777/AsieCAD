#version 330

layout(points) in;
layout(line_strip, max_vertices = 512) out;

uniform mat4 Knots[3];
uniform bool isForward;
uniform mat4 viewProjection;
float intval = 1.0/3.0;

float spline0(float t, float ti)
{
    if(ti>t && ti-intval<=t)
        return 1;
    else
        return 0;
}

float spline1(float t, float ti)
{
    float val1 = spline0(t, ti) * (t - ti + intval);
    float val2 = spline0(t, ti + intval) * (ti + intval - t);
    return (val1 + val2) / intval;
}
float spline2(float t, float ti)
{
    float val1 = spline1(t, ti) * (t - ti + intval);
    float val2 = spline1(t, ti + intval) * (ti + 2 * intval - t);
    return (val1 + val2) / (2 * intval);
}
float spline3(float t, float ti)
{
    float val1 = spline2(t, ti) * (t - ti + intval);
    float val2 = spline2(t, ti + intval) * (ti + 3 * intval - t);
    return (val1 + val2) / (3 * intval);
}

vec4 toSplineVec(float t)
{
    return vec4(spline3(t, 0), spline3(t, intval), spline3(t, 2 * intval), spline3(t, 1));
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
    float u = gl_in[0].gl_Position.x / 3.0 + 2.0 / 3.0;
    vec4 uCoord = toSplineVec(u);
    mat4 knots[3];
    if (isForward)
        for(int i = 0; i < 3; i++)
            knots[i] = Knots[i];            
    else
        for(int i = 0; i < 3; i++)
            knots[i] = transpose(Knots[i]);

    vec4 xCoord = knots[0] * uCoord;
    vec4 yCoord = knots[1] * uCoord;
    vec4 zCoord = knots[2] * uCoord;
    vec4 B[4], Boors[4];
    Boors[0] = vec4(xCoord.x, yCoord.x, zCoord.x, 1);
    Boors[1] = vec4(xCoord.y, yCoord.y, zCoord.y, 1);
    Boors[2] = vec4(xCoord.z, yCoord.z, zCoord.z, 1);
    Boors[3] = vec4(xCoord.w, yCoord.w, zCoord.w, 1);

    B[0] = Boors[0] * 1.f / 3.f + Boors[1] * 2.f / 3.f;
	B[1] = Boors[1] * 2.f / 3.f + Boors[2] * 1.f / 3.f;
	B[2] = Boors[1] * 1.f / 3.f + Boors[2] * 2.f / 3.f;
	B[3] = Boors[2] * 2.f / 3.f + Boors[3] * 1.f / 3.f;
	B[0] = B[0] * 1.f / 2.f + B[1] * 1.f / 2.f;
	B[3] = B[2] * 1.f / 2.f + B[3] * 1.f / 2.f;
    for (int i = 0; i < 4; i++)
    {
        B[i] = viewProjection * B[i];
        //gl_Position = B[i];
        //EmitVertex();
    }
    float dist = distance(B[0].xy / B[0].w, B[1].xy / B[1].w) + 
        distance(B[1].xy / B[1].w, B[2].xy / B[2].w) + 
        distance(B[2].xy / B[2].w, B[3].xy / B[3].w) + 0.04;
    
    int steps = min(int(dist * 30), 511);
    float delta = 1.0 / float(steps);
    for (int i=0; i<=steps; ++i){
        gl_Position = toBezier3(delta, i, B[0], B[1], B[2], B[3]);
        EmitVertex();
    }
}