#version 330 core

layout(location = 0) in vec4 in_coord;

uniform mat4 u_projection;

out vec2 sent_texCoord;

void main() {
    sent_texCoord = in_coord.zw;
    gl_Position = u_projection * vec4(in_coord.xy, -0.1, 1.0);
}
