#version 330 core

in vec3 sent_color;
in vec2 sent_texCoord;

uniform bool u_useTexture;
uniform sampler2D u_texture;

out vec4 final_color;

void main() {
    if (u_useTexture) {
        final_color = texture(u_texture, sent_texCoord) * vec4(sent_color, 1.0);
    } else {
        final_color = vec4(sent_color, 1.0);
    }
}
