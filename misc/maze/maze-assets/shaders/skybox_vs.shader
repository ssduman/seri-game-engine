#version 330 core

layout(location = 0) in vec3 in_coord;

uniform vec3 u_position = vec3(0.0, 0.0, 0.0);
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 sent_texCoord;

void main() {
    sent_texCoord = in_coord;
    
    vec4 position = u_projection * u_view * u_model * vec4(in_coord + u_position, 1.0);
    gl_Position = position.xyww;
}
