#version 330 core

in struct_info {
    vec3 sent_pos;
    vec3 sent_normal;
    vec2 sent_texCoord;
} received_data;

uniform vec3 u_viewPos;
uniform vec3 u_lightPos;
uniform float u_ambient;
uniform sampler2D u_texture;

out vec4 final_color;

void main() {
    vec3 lightColor = vec3(1);
    vec3 normal = normalize(received_data.sent_normal);
    vec3 color = texture(u_texture, received_data.sent_texCoord).rgb;
    vec3 ambient = u_ambient * color;

    vec3 lightDir = normalize(u_lightPos - received_data.sent_pos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(u_viewPos - received_data.sent_pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * color;
    final_color = vec4(lighting, 1.0);
}
