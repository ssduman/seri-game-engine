#version 330 core

layout(location = 0) in vec3 in_coord;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_texCoord;

out vec3 sent_color;
out vec2 sent_texCoord;

void main() {
    sent_color = in_color;
    sent_texCoord = in_texCoord;
    
    gl_Position = vec4(in_coord, 1.0);
}
