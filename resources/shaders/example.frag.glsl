#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

struct Material {
    vec4 baseColorFactor;
    float metallicFactor;
    float roughnessFactor;
    float ambientFactor;
    vec3 emissiveFactor;
    vec3 ambientLight;

    sampler2D baseColorTexture;
    sampler2D ambientTexture;
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    sampler2D emissionTexture;
    sampler2D metallicTexture;
    sampler2D roughnessTexture;
    sampler2D occlusionTexture;
    sampler2D normalTexture;
};

uniform vec3 viewPos;
uniform Material material;

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

void main()
{
    FragColor = texture(material.baseColorTexture, TexCoords);
    // // NOTE: the fellow lines were disabled, because of no light objects.
    // vec4 albedo = texture(material.baseColorTexture, TexCoords) * material.baseColorFactor;
    
    // if (albedo.a < 0.1) discard;

    // vec3 emissionColor = texture(material.emissionTexture, TexCoords).rgb * material.emissiveFactor;
    // vec3 normal = texture(material.normalTexture, TexCoords).rgb;
    // vec3 armValue = texture(material.armTexture, TexCoords).rgb;

    // normal = normalize(normal * 2.0 - 1.0);
    // normal = normalize(TBN * normal);

    // float roughness = armValue.g * material.roughnessFactor;
    // float metallic = armValue.b * material.metallicFactor;

    // vec3 V = normalize(viewPos - FragPos);
    // vec3 N = normal;

    // vec3 F0 = mix(vec3(0.04), albedo.rgb, metallic);

    // vec3 Lo = vec3(0.0);
    // Lo = albedo.rgb;
    
    // calculate for directional light

    // calculate for point light

    // calculate for spot light

    // vec3 color = emissionColor + Lo;

    // mapping
    // color = vec3(1.0) - exp(-color * exposure);

    // FragColor = vec4(color, albedo.a);
}
