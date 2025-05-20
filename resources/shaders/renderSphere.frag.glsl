#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

#define kMaxPointLightCount 15

struct Material {
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    float ambientFactor;
    vec3 emissiveFactor;
    vec3 ambientLight;

    sampler2D baseColorTexture;
    sampler2D emissionTexture;
    sampler2D metallicTexture;
    sampler2D roughnessTexture;
    sampler2D occlusionTexture;
    sampler2D normalTexture;
};

struct DirLight {
    vec3 direction;
    vec4 color; //r,g,b,intensity
};

struct PointLight {
    vec3 position;
    vec3 color;

    float intensity;

    float constant;
    float linearValue;
    float quadratic;
};

uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirLight;
uniform int pointLightCount;
uniform PointLight pointLights[kMaxPointLightCount];

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = (max(dot(N, H), 0.0));
    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 calculateLight(vec3 V, vec3 N, vec3 L, vec3 H, float roughness, float metallic, vec3 F0, vec3 albedo, vec3 lightColor, float intensity) {
    float D = DistributionGGX(N, H, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 kD = (1.0 - F) * (1.0 - metallic);
    vec3 diffuse = kD * albedo / PI;

    vec3 numerator = D * F * G;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = numerator / denominator;
    
    vec3 radiance = lightColor * intensity;
    return (diffuse + specular) * radiance * max(dot(N, L), 0.0);
}

vec3 calculateDirLight(DirLight light, vec3 V, vec3 N, float roughness, float metallic, vec3 F0, vec3 albedo) {
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(L + V);

    return calculateLight(V, N, L, H, roughness, metallic, F0, albedo, light.color.rgb, light.color.a);
}

vec3 calculatePointLight(PointLight light, vec3 V, vec3 N, float roughness, float metallic, vec3 F0, vec3 albedo) {
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(L + V);

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + (light.linearValue + light.quadratic * dist) * dist);

    return calculateLight(V, N, L, H, roughness, metallic, F0, albedo, light.color, light.intensity);
}

void main() {
    vec4 albedo = texture(material.baseColorTexture, TexCoords);

    // if (albedo.a < 0.1) discard;

    vec3 emissionColor = texture(material.emissionTexture, TexCoords).rgb;

    float roughness = texture(material.roughnessTexture, TexCoords).g;
    float metallic = texture(material.metallicTexture, TexCoords).b;

    vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

    vec3 V = normalize(viewPos - FragPos);
    vec3 N = normalize(Normal);

    vec3 Lo = vec3(0.0);
    Lo += calculateDirLight(dirLight, V, N, roughness, metallic, F0, albedo.rgb);
    for (int i = 0; i < pointLightCount; ++i) {
        Lo = Lo + calculatePointLight(pointLights[i], V, N, roughness, metallic, F0, albedo.rgb);
    }

    vec3 color = emissionColor + Lo;

    FragColor = vec4(color, albedo.a);
    FragColor = vec4(Lo, 1.0);
    // FragColor = vec4(1.0);
}

