#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;
layout(location = 3) in vec2 in_normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 sent_uv;
out vec4 sent_color;

void main()
{
    sent_uv = in_uv;
    sent_color = in_color;

    gl_Position = u_projection * u_view * u_model * vec4(in_vertex, 1.0);
}
