#beg_vs

#version 460 core

layout(location = 0) in vec3 in_vertex;
layout(location = 1) in vec2 in_uv0;
layout(location = 2) in vec3 in_normal;
layout(location = 7) in mat4 in_model_instanced;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec4 sent_pos;
out vec2 sent_uv;
out vec3 sent_normal;

void main()
{
    sent_uv = in_uv0;
    sent_pos = in_model_instanced * vec4(in_vertex, 1.0);
    sent_normal = normalize(mat3(transpose(inverse(in_model_instanced))) * in_normal);

    gl_Position = u_projection * u_view * sent_pos;
}

#end_vs

#beg_fs

#version 460 core

in vec4 sent_pos;
in vec2 sent_uv;
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