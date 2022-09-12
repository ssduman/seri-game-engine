#version 330 core

layout(location = 0) in vec3 in_coord;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_move;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out struct_info {
    vec3 sent_pos;
    vec3 sent_normal;
    vec2 sent_texCoord;
} sent_data;

void main() {
    sent_data.sent_pos = vec3(u_model * vec4(in_coord + in_move, 1.0));
    sent_data.sent_normal = transpose(inverse(mat3(u_model))) * in_normal;
    sent_data.sent_texCoord = in_texCoord;
    gl_Position = u_projection * u_view * u_model * vec4(in_coord + in_move, 1.0);
}
