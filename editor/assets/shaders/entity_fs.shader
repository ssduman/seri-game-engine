#version 460 core

in vec4 sent_color;
in vec2 sent_texCoord;

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;
uniform bool u_useColors = false;
uniform bool u_useTexture = false;

out vec4 final_color;

void main() {
    final_color = u_color;
    
    if (u_useTexture && u_useColors) {
        final_color *= texture(u_texture, sent_texCoord) * sent_color;
    }
    else if (u_useTexture) {
        final_color *= texture(u_texture, sent_texCoord);
    }
    else if (u_useColors) {
        final_color *= sent_color;
    }
}
