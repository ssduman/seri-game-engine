#beg_vs

#version 330 core

layout(location = 0) in vec4 in_vertex;

uniform mat4 u_projection;

out vec2 sent_texCoord;

void main()
{
    sent_texCoord = in_vertex.zw;
    gl_Position = u_projection * vec4(in_vertex.xy, 0.0, 1.0);
}

#end_vs

#beg_fs

#version 330 core

in vec2 sent_texCoord;

uniform vec4 u_color;
uniform sampler2D u_texture;

out vec4 final_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_texture, sent_texCoord).r);
    final_color = u_color * sampled;
}

#end_fs