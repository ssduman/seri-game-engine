#beg_vs

#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec2 sent_uv;

void main()
{
    sent_uv = in_uv;

    gl_Position = u_projection * u_view * u_model vec4(in_vertex.xy, 0.0, 1.0);
}

#end_vs

#beg_fs

#version 460 core

in vec2 sent_uv;

uniform sampler2D u_texture;

out vec4 final_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, sent_uv).r);
    final_color = sampled;
}

#end_fs