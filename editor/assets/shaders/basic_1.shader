#beg_vs

#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 2) in vec4 in_color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec4 sent_color;

void main()
{
    sent_color = in_color;

    gl_Position = u_projection * u_view * u_model * vec4(in_vertex, 1.0);
}

#end_vs

#beg_fs

#version 460 core

in vec4 sent_color;

out vec4 final_color;

void main()
{
    final_color = sent_color;
}

#end_fs