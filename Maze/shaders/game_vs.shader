#version 330 core

layout (location = 0) in vec4 aPos;

out vec2 TexCoord;

uniform mat4 projection;

void main() {
	gl_Position = projection * vec4(aPos.xy, -0.1, 1.0);
	TexCoord = aPos.zw;
}