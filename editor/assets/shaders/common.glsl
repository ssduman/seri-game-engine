#ifndef SERI_COMMON_GLSL
#define SERI_COMMON_GLSL

#define MAX_POINT_LIGHTS 16

const float PI = 3.14159265359;

struct PointLight
{
    vec3 position;
    vec3 color;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
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
    vec3 tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;
    
    vec3 Q1 = dFdx(fragPos);
    vec3 Q2 = dFdy(fragPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);
    
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
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

vec3 PBR_Dir_Light(DirectionalLight dirLight, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic)
{
    vec3 L = normalize(-dirLight.direction);
    vec3 radiance = dirLight.color;

    return BRDF_PBR(N, V, L, radiance, albedo, roughness, metallic);
}

vec3 PBR_Point_Light(vec3 pos, PointLight pointLight, vec3 N, vec3 V, vec3 albedo, float roughness, float metallic)
{
    vec3 lightDiff = pointLight.position - pos;
    float lightDist = length(lightDiff);
    vec3 L = lightDiff / max(lightDist, 0.001);

    float attenuation = 1.0 / max(lightDist * lightDist, 0.01);
    vec3 radiance = pointLight.color * attenuation;

    return BRDF_PBR(N, V, L, radiance, albedo, roughness, metallic);
}

#endif
