#version 330 core

layout(location = 0) in vec3 in_coord;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 sent_texCoord;

void main() {
    sent_texCoord = in_coord;
    vec4 position = u_projection * u_view * vec4(in_coord, 1.0);
    gl_Position = position.xyww;
}