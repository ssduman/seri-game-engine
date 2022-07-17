#version 330 core

out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D Texture;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientS;

void main() {
    vec3 color = texture(Texture, fs_in.TexCoords).rgb;

    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(1);

    vec3 ambient = ambientS * color;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular) * color;
    FragColor = vec4(lighting, 1.0);
}