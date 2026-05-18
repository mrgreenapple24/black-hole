#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in vec4 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec4 PartColor;

void main() {
    vec4 viewPos = view * vec4(aPos, 1.0);
    gl_Position = projection * viewPos;
    
    float dist = length(viewPos.xyz);
    gl_PointSize = (aSize * 800.0) / dist;
    
    PartColor = aColor;
}