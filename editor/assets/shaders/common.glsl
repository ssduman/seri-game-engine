#ifndef SERI_COMMON_GLSL
#define SERI_COMMON_GLSL

#define MAX_SPOT_LIGHTS 8
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHT_SHADOWS 4

const float PI = 3.14159265359;

struct DirectionalLight
{
    vec4 direction;
    vec4 color;
};

struct SpotLight
{
    vec4 position;
    vec4 direction;
    vec4 color;
    vec4 params;  // x = innerCos, y = outerCos, z = constant, w = linear
    vec4 params2; // x = quadratic, y/z/w = unused
};

struct PointLight
{
    vec4 position;
    vec4 color;
    vec4 params; // x = constant, y = linear, z = quadratic, w = unused
};

// layout(std140, binding = 0) uniform CameraBuffer
// {
//     mat4 u_view;
//     mat4 u_proj;
//     mat4 u_view_proj;
//     vec4 u_camera_pos;
//     vec4 u_time; // x = time, y = deltaTime, z and w unused
// };

layout(std140, binding = 1) uniform LightBuffer
{
    DirectionalLight u_dir_light;
    ivec4 u_dir_light_exist;

    SpotLight u_spot_lights[MAX_SPOT_LIGHTS];
    ivec4 u_spot_light_count;

    PointLight u_point_lights[MAX_POINT_LIGHTS];
    ivec4 u_point_light_count;
};

layout(std140, binding = 3) uniform ShadowBuffer
{
    vec4 u_shadow_modifiers; // x=shadow bias, y=normal bias, z=shadow strength, w=unused

    mat4 u_dir_light_view_proj;

    mat4 u_spot_light_view_proj[MAX_SPOT_LIGHT_SHADOWS];
    ivec4 u_spot_light_shadow_count;
};

float Saturate(float x) 
{ 
    return clamp(x, 0.0, 1.0);
}

vec3 SRGBToLinear(vec3 c)
{
    return pow(c, vec3(2.2));
}

vec3 LinearToSRGB(vec3 c)
{
    return pow(c, vec3(1.0 / 2.2));
}

vec3 Reinhard(vec3 c)
{
    return c / (c + vec3(1.0));
}

vec3 ACESFilm(vec3 x)
{
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;

    float NdotH = Saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom + 1e-7;
    
    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k + 1e-7;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = Saturate(dot(N, V));
    float NdotL = Saturate(dot(N, L));
    
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - Saturate(cosTheta), 5.0);
}

vec3 GetNormal(sampler2D normalMap, vec2 texCoords, vec3 N, vec3 fragPos)
{
    vec3 sampled = texture(normalMap, texCoords).xyz;

    if (dot(sampled, sampled) < 0.01)
    {
        return N;
    }

    vec3 tangentNormal = sampled * 2.0 - 1.0;

    vec3 Q1 = dFdx(fragPos);
    vec3 Q2 = dFdy(fragPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);

    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float ShadowDir(sampler2DShadow shadowMap, vec3 pos, vec3 N, vec3 L)
{
    vec4 ls = u_dir_light_view_proj * vec4(pos, 1.0);
    vec3 p = ls.xyz / ls.w;
    p = p * 0.5 + 0.5;

    if (p.x < 0 || p.x > 1 || p.y < 0 || p.y > 1 || p.z > 1)
    {
        return 1.0;
    }

    float bias = u_shadow_modifiers.x + u_shadow_modifiers.y * (1.0 - max(dot(N, L), 0.0));
    float depth = p.z - bias;

    // fade out shadow near frustum edges
    float fadeRange = 0.1;
    float fade = min(
        min(smoothstep(0.0, fadeRange, p.x), smoothstep(0.0, fadeRange, 1.0 - p.x)),
        min(smoothstep(0.0, fadeRange, p.y), smoothstep(0.0, fadeRange, 1.0 - p.y))
    );

    // 5x5 PCF for smooth shadow edges
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            shadow += texture(shadowMap, vec3(p.xy + vec2(x, y) * texelSize, depth));
        }
    }
    return mix(1.0, shadow / 25.0, fade);
}

float ShadowSpot(sampler2DShadow shadowMap, mat4 lightViewProj, vec3 pos, vec3 N, vec3 L)
{
    vec4 ls = lightViewProj * vec4(pos, 1.0);
    vec3 p = ls.xyz / ls.w;
    p = p * 0.5 + 0.5;

    if (p.x < 0 || p.x > 1 || p.y < 0 || p.y > 1 || p.z > 1)
    {
        return 1.0;
    }

    float bias = 0.001 + 0.003 * (1.0 - max(dot(N, L), 0.0));
    float depth = p.z - bias;

    // 3x3 PCF
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            // shadow += texture(shadowMap, vec3(p.xy + vec2(x, y) * texelSize, depth));
        }
    }
    return shadow / 9.0;
}

vec3 BRDF_PBR(vec3 N, vec3 V, vec3 L, vec3 radiance, vec3 albedo, float roughness, float metallic)
{
    vec3 H = normalize(V + L);
    
    float NdotL = Saturate(dot(N, L));
    float NdotV = Saturate(dot(N, V));
    float HdotV = Saturate(dot(H, V));

    if (NdotL <= 0.0)
    {
        return vec3(0.0);
    }

    vec3 F0 = mix(vec3(0.04), albedo, metallic);

    float D = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3  F = FresnelSchlick(HdotV, F0);
    
    vec3 numerator = D * G * F;
    float denom = 4.0 * max(NdotV, 1e-4) * max(NdotL, 1e-4);
    vec3 specular = numerator / max(denom, 1e-7);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    vec3 diffuse = (kD * albedo) / PI;

    return (diffuse + specular) * radiance * NdotL;
}

vec3 PBR_Dir_Light(vec3 pos, DirectionalLight dirLight, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic)
{
    vec3 L = normalize(-dirLight.direction.xyz);
    vec3 radiance = dirLight.color.rgb;

    return BRDF_PBR(N, V, L, radiance, albedo, roughness, metallic);
}

vec3 PBR_Spot_Light(vec3 pos, SpotLight spotLight, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic)
{
    vec3 lightDiff = spotLight.position.xyz - pos;
    float lightDist = length(lightDiff);
    vec3 L = lightDiff / max(lightDist, 0.001);

    vec3 lightDir = normalize(spotLight.direction.xyz);
    float theta = dot(L, -lightDir);

    float epsilon = max(spotLight.params.x - spotLight.params.y, 0.001);
    float spot = clamp((theta - spotLight.params.y) / epsilon, 0.0, 1.0);

    float constant = spotLight.params.z;
    float linear = spotLight.params.w;
    float quadratic = spotLight.params2.x;
    float attenuation = 1.0 / max(constant + linear * lightDist + quadratic * lightDist * lightDist, 0.01);
    vec3 radiance = spotLight.color.rgb * attenuation * spot;

    return BRDF_PBR(N, V, L, radiance, albedo, roughness, metallic);
}

vec3 PBR_Point_Light(vec3 pos, PointLight pointLight, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic)
{
    vec3 lightDiff = pointLight.position.xyz - pos;
    float lightDist = length(lightDiff);
    vec3 L = lightDiff / max(lightDist, 0.001);

    float constant = pointLight.params.x;
    float linear = pointLight.params.y;
    float quadratic = pointLight.params.z;
    float attenuation = 1.0 / max(constant + linear * lightDist + quadratic * lightDist * lightDist, 0.01);
    
    vec3 radiance = pointLight.color.rgb * attenuation;

    return BRDF_PBR(N, V, L, radiance, albedo, roughness, metallic);
}

#endif