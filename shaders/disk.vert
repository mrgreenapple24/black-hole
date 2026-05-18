#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNorm;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;
out vec3 LocalPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isJet;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    LocalPos = aPos;
    if (!isJet) {
        TexCoords = aTex;
        Normal = mat3(transpose(inverse(model))) * aNorm;
    } else {
        TexCoords = vec2(0.0);
        Normal = vec3(0,1,0);
    }
    gl_Position = projection * view * vec4(FragPos, 1.0);
}