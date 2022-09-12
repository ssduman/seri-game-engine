#version 330 core

in vec2 sent_texCoord;

uniform sampler2D u_texture;

out vec4 final_color;

void main() {
    final_color = texture(u_texture, sent_texCoord);
}
