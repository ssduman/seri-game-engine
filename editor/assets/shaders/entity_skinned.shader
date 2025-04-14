#beg_vs

#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec4 in_color;
layout(location = 3) in vec3 in_normal;
layout(location = 4) in ivec4 in_bone_ids;
layout(location = 5) in vec4 in_weights;

const int MAX_BONES = 200;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform mat4 u_bones[MAX_BONES];

out vec2 sent_uv;
out vec4 sent_color;
out vec3 sent_normal;

void main()
{
    sent_uv = in_uv;
    sent_color = in_color;
    sent_normal = normalize(mat3(transpose(inverse(u_model))) * in_normal);

    mat4 boneTransform = 
        u_bones[in_bone_ids[0]] * in_weights[0] +
        u_bones[in_bone_ids[1]] * in_weights[1] +
        u_bones[in_bone_ids[2]] * in_weights[2] +
        u_bones[in_bone_ids[3]] * in_weights[3];

    gl_Position =  u_projection * u_view * u_model * (boneTransform * vec4(in_vertex, 1.0));
}

#end_vs

#beg_fs

#version 460 core

in vec2 sent_uv;
in vec4 sent_color;
in vec3 sent_normal;

uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;

out vec4 final_color;

void main()
{
    final_color = texture(u_texture, sent_uv);
}

#end_fs