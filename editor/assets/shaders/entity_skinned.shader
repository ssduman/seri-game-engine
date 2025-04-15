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

out vec4 sent_pos;
out vec2 sent_uv;
out vec4 sent_color;
out vec3 sent_normal;

void main()
{
    mat4 bone_transform = 
        u_bones[in_bone_ids[0]] * in_weights[0] +
        u_bones[in_bone_ids[1]] * in_weights[1] +
        u_bones[in_bone_ids[2]] * in_weights[2] +
        u_bones[in_bone_ids[3]] * in_weights[3]
    ;
    
    sent_uv = in_uv;
    sent_color = in_color;
    sent_pos = bone_transform * vec4(in_vertex, 1.0);
    sent_normal = normalize(mat3(transpose(inverse(u_model * bone_transform))) * in_normal);

    gl_Position =  u_projection * u_view * u_model * sent_pos;
}

#end_vs

#beg_fs

#version 460 core

in vec4 sent_pos;
in vec2 sent_uv;
in vec4 sent_color;
in vec3 sent_normal;

uniform vec3 u_view_pos;
uniform vec3 u_light_dir;
uniform vec4 u_light_color;
uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform sampler2D u_texture;

out vec4 final_color;

void main()
{
    vec3 light_dir_norm = normalize(-u_light_dir);

    float ambient_strength = 0.2;
    vec3 ambient = ambient_strength * u_light_color.xyz;

    float specular_strength = 0.5;
    vec3 view_dir = normalize(u_view_pos - sent_pos.xyz);
    vec3 reflect_dir = reflect(-light_dir_norm, sent_normal);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * u_light_color.xyz;

    float diffuse_strength = 0.9;
    vec3 diffuse = diffuse_strength * max(dot(sent_normal, light_dir_norm), 0.0) * u_light_color.xyz;

    final_color = texture(u_texture, sent_uv) * vec4(clamp(ambient + diffuse + specular, 0.0, 1.0), 1.0);
}

#end_fs