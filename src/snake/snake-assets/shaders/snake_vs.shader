#version 330 core

layout(location = 0) in vec2 in_coord;
layout(location = 1) in vec2 in_texCoord;

uniform vec2 u_position = vec2(0.0, 0.0);
uniform mat4 u_projection;

out vec2 sent_texCoord;

void main() {
    sent_texCoord = in_texCoord;

    gl_Position = u_projection * vec4(in_coord + u_position, -0.1, 1.0);
}
