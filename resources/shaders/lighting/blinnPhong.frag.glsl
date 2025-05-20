#version 410 core
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

const float kA = 0.1;
const float kD = 0.8;
const float kS = 0.5;

vec3 calculateDirLight(DirLight light, vec3 N, vec3 V, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(L + V);

    // vec3 a = ambient * light.kA;
    // vec3 d = diffuse * light.kD * max(dot(N, L), 0.0);
    // vec3 s = specular * light.kS * pow(max(dot(N, H), 0.0), shininess);
    vec3 a = ambient * kA;
    vec3 d = diffuse * kD * max(dot(N, L), 0.0);
    vec3 s = specular * kS * pow(max(dot(N, H), 0.0), shininess);

    return a + d + s; 
}

vec3 calculatePointLight(PointLight light, vec3 N, vec3 V, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
    vec3 L = normalize(light.position - FragPos);
    vec3 H = normalize(L + V);

    // vec3 a = ambient * light.kA;
    // vec3 d = diffuse * light.kD * max(dot(N, L), 0.0);
    // vec3 s = specular * light.kS * pow(max(dot(N, H), 0.0), shininess);
    vec3 a = ambient * kA;
    vec3 d = diffuse * kD * max(dot(N, L), 0.0);
    vec3 s = specular * kS * pow(max(dot(N, H), 0.0), shininess);

    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + (light.linearValue + (light.quadratic * dist) * dist));

    return (a + d + s) * attenuation;
}

void main() {
    vec4 diffuse = texture(material.diffuseTexture, TexCoords);

    // if (diffuse.a < 0.1) discard;

    vec3 ambient = texture(material.ambientTexture, TexCoords).rgb;
    vec3 specular = texture(material.specularTexture, TexCoords).rgb;

    vec3 V = normalize(viewPos - FragPos);

    // vec3 norm = texture(material.normalTexture, TexCoords).rgb;
    vec3 N = Normal;

    vec3 color = vec3(0.0);
    color += calculateDirLight(dirLight, N, V, ambient, diffuse.rgb, specular, 32);
    for (int i = 0; i < pointLightCount; ++i) {
        color += calculatePointLight(pointLights[i], N, V, ambient, diffuse.rgb, specular, 32);
    }

    FragColor = vec4(color, diffuse.a);
}