#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 t_coord_;
layout (location = 2) in vec3 d;

out vec2 t_coord;

uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(position.x + d.x, position.y + d.y, position.z + d.z, 1.0);
	t_coord = t_coord_;
}
