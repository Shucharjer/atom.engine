#version 460 core
#define max_point_light 15
#define max_spot_light 15
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec3 Normal;

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
uniform PointLight pointLights[max_point_light];


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

vec3 calculateDirLight(DirLight dirLight, vec3 viewDir, vec3 normal) {
    vec3 lightDir = normalize(-dirLight.direction);

    return max(dot(normal, lightDir),0) * dirLight.color.rgb * dirLight.color.a;
}

vec3 calculatePointLight(PointLight pointLight, vec3 fragPos, vec3 viewDir, vec3 normal) {
    vec3 lightDir = normalize(pointLight.position - fragPos);
    float distance = length(pointLight.position - fragPos);
    
    float attenuation = 1.0 / (pointLight.constant + 
                              pointLight.linearValue * distance + 
                              pointLight.quadratic * distance * distance);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    return diff * pointLight.intensity * attenuation * pointLight.color;
}

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec4 texColor = texture(material.diffuseTexture, TexCoords);
    vec3 color = calculateDirLight(dirLight, viewDir, Normal);
    for (int i = 0; i < pointLightCount; i++) {
        color += calculatePointLight(pointLights[i], FragPos, viewDir, Normal);
    }
    FragColor = vec4(color * texColor.rgb, texColor.a);
}
