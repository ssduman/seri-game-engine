#version 330 core

in vec3 sent_texCoord;

uniform samplerCube u_skybox;

out vec4 final_color;

void main() {
    final_color = texture(u_skybox, sent_texCoord);
}