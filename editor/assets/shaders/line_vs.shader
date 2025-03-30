#version 460 core

layout(location = 0) in vec3 in_vertex;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    gl_Position = vec4(in_vertex, 1.0);
}
