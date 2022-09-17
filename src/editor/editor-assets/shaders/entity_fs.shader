#version 330 core

in vec4 sent_color;
in vec2 sent_texCoord;

uniform vec4 u_color;
uniform bool u_useColor = false;
uniform bool u_useColors = false;
uniform sampler2D u_texture;
uniform bool u_useTexture = false;

out vec4 final_color;

void main() {
    if (u_useTexture && u_useColor && u_useColors) {
        final_color = texture(u_texture, sent_texCoord) * u_color * sent_color;
    }
    if (u_useTexture && u_useColor) {
        final_color = texture(u_texture, sent_texCoord) * u_color;
    }
    else if (u_useTexture && u_useColors) {
        final_color = texture(u_texture, sent_texCoord) * sent_color;
    }
    else if (u_useColor && u_useColors) {
        final_color = u_color * sent_color;
    }
    else if (u_useTexture) {
        final_color = texture(u_texture, sent_texCoord);
    }
    else if (u_useColor) {
        final_color = u_color;
    }
    else if (u_useColors) {
        final_color = sent_color;
    }
    else {
        final_color = vec4(0.1, 0.4, 0.9, 1.0f);
    }
}
