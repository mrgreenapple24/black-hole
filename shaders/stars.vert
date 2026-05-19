#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aSize;
layout (location = 2) in vec3 aColor;

out vec3 vColor;

uniform mat4 view;
uniform mat4 projection;
uniform float uViewHeight;
uniform float uSize;

void main() {
    vColor = aColor;
    vec4 modelViewPosition = view * vec4(aPos, 1.0);
    gl_Position = projection * modelViewPosition;
    gl_PointSize = aSize * uSize * uViewHeight;
}
