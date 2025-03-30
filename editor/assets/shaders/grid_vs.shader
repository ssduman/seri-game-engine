#version 460 core

layout(location = 0) in vec3 in_vertex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 sent_near_point;
out vec3 sent_far_point;
out mat4 sent_view;
out mat4 sent_proj;

// source: https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

vec3 UnprojectPoint(mat4 view_inv, mat4 proj_inv, float x, float y, float z)
{
    vec4 unprojectedPoint =  view_inv * proj_inv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    mat4 view_inv = inverse(u_view);
    mat4 proj_inv = inverse(u_projection);

    vec3 p = in_vertex;
    
    sent_near_point = UnprojectPoint(view_inv, proj_inv, p.x, p.y, 0.0).xyz;
    sent_far_point = UnprojectPoint(view_inv, proj_inv, p.x, p.y, 1.0).xyz;
    sent_view = u_view;
    sent_proj = u_projection;

    gl_Position = vec4(in_vertex.xyz, 1.0);
}
