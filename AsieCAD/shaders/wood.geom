#version 330

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in VS_OUT {
	vec3 FragPos;
} gs_in[];

out vec3 FragPos;
out vec3 Normal;
out vec2 texCoords;

vec3 GetNormal()
{
   vec3 a = vec3(gs_in[0].FragPos) - vec3(gs_in[1].FragPos);
   vec3 b = vec3(gs_in[2].FragPos) - vec3(gs_in[1].FragPos);
   return normalize(cross(a, b));
}  

void main(void)
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 normal = GetNormal();

    gl_Position = gl_in[0].gl_Position;
    Normal = normal;
    FragPos = gs_in[0].FragPos;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    Normal = normal;
    FragPos = gs_in[1].FragPos;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    Normal = normal;
    FragPos = gs_in[2].FragPos;
    EmitVertex();

    EndPrimitive();
}  

