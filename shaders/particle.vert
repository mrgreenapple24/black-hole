#version 330 core
layout (location = 0) in float aDistance;
layout (location = 1) in float aSize;
layout (location = 2) in float aRandom;

out vec3 vColor;

uniform mat4 view;
uniform mat4 projection;
uniform float uTime;
uniform vec3 uInnerColor;
uniform vec3 uOuterColor;
uniform float uViewHeight;
uniform float uSize;

void main() {
    float concentration = 0.05;
    float outerProgress = smoothstep(0.0, 1.0, aDistance);
    outerProgress = mix(concentration, outerProgress, pow(aRandom, 1.7));
    float radius = 1.0 + outerProgress * 5.0;
    
    float angle = outerProgress - uTime * (1.0 - outerProgress) * 3.0;
    vec3 newPosition = vec3(
        sin(angle) * radius,
        0.0,
        cos(angle) * radius
    );
    
    vec4 modelViewPosition = view * vec4(newPosition, 1.0);
    gl_Position = projection * modelViewPosition;

    gl_PointSize = aSize * uSize * uViewHeight;
    gl_PointSize *= (1.0 / - modelViewPosition.z);

    vColor = mix(uInnerColor, uOuterColor, outerProgress);
}
