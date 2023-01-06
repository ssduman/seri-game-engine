#version 330 core

in vec2 sent_texCoord;

uniform vec4 u_color;
uniform sampler2D u_texture;

out vec4 final_color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, sent_texCoord).r);
    final_color = u_color * sampled;
}