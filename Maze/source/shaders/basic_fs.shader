#version 330 core

out vec4 FragColor;
in vec2 t_coord;

uniform sampler2D oursampler;

void main() {
	vec4 tc = texture(oursampler, t_coord);
	FragColor = tc;
}
