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
    vec3 newPosition;
    float outerProgress;
    
    if (aRandom < 5.0) {
        // Disk Particles
        float concentration = 0.05;
        outerProgress = smoothstep(0.0, 1.0, aDistance);
        outerProgress = mix(concentration, outerProgress, pow(aRandom, 1.7));
        float radius = 1.0 + outerProgress * 5.0;
        
        float angle = outerProgress - uTime * (1.0 - outerProgress) * 3.0;
        newPosition = vec3(
            sin(angle) * radius,
            0.0,
            cos(angle) * radius
        );
    } else {
        // 3D Cloud Particles
        float rand = aRandom - 10.0;
        float radius = aDistance;
        float theta = rand * 6.28 + uTime * 0.1;
        float phi = fract(rand * 7.89) * 3.14;
        
        newPosition = vec3(
            radius * sin(phi) * cos(theta),
            radius * sin(phi) * sin(theta),
            radius * cos(phi)
        );
        outerProgress = (radius - 1.0) / 10.0;
    }
    
    vec4 modelViewPosition = view * vec4(newPosition, 1.0);
    gl_Position = projection * modelViewPosition;

    gl_PointSize = aSize * uSize * uViewHeight;
    gl_PointSize *= (1.0 / - modelViewPosition.z);

    vColor = mix(uInnerColor, uOuterColor, outerProgress);
}
