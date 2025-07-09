#version 330 core

layout(location = 0) in vec4 in_vertex;

uniform mat4 u_projection;

out vec2 sent_texCoord;

void main() {
    sent_texCoord = in_vertex.zw;
    gl_Position = u_projection * vec4(in_vertex.xy, 0.0, 1.0);
}
