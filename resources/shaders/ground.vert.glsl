#version 410

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 ViewPos;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 proj;

void main() {
    FragPos = vec3(viewPos.x + aPos.x, 0, viewPos.z + aPos.z);
    TexCoords = aTexCoords;
    gl_Position = proj * view * vec4(FragPos, 1.0);
    ViewPos = viewPos;
}