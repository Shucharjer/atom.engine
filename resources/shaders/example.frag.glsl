#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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
};

uniform vec3 viewPos;
uniform Material material;

void main()
{
    vec4 ambientColor = texture(material.ambientTexture, TexCoords);
    vec4 diffuseColor = texture(material.diffuseTexture, TexCoords);
    vec4 specularColor = texture(material.specularTexture, TexCoords);
    vec4 emissionColor = texture(material.emissionTexture, TexCoords);

    FragColor = diffuseColor;
}
