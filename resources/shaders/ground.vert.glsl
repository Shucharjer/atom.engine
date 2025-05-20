#version 410

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

uniform vec3 viewPos;

void main() {
    vec3 pos = viewPos;
    pos.x += 
}