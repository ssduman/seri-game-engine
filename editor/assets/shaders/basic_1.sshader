#beg_vs

#version 460 core

layout(location = 0) in vec3 in_vertex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(in_vertex, 1.0);
}

#end_vs

#beg_fs

#version 460 core

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);

out vec4 final_color;

void main()
{
    final_color = u_color;
}

#end_fs