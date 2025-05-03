#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoords;

out vec3 FragPos;
out vec3 TexCoords;
out vec3 Normal;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void main()
{
    Normal = aNormal;
    TexCoords = aTexCoords;
    FragPos = vec3(model * vec4(aPos, 1.0));
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}