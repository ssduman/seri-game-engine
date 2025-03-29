#version 460 core

in vec2 sent_uv;
in vec4 sent_color;

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;

out vec4 final_color;

void main() {
    final_color = texture(u_texture, sent_uv);
}
