#version 330 core
out vec4 FragColor;
in vec2 vUv;

uniform float uTime;
uniform sampler2D uNoiseTexture;
uniform vec3 uInnerColor;
uniform vec3 uOuterColor;

float inverseLerp(float v, float minValue, float maxValue) {
    return (v - minValue) / (maxValue - minValue);
}

vec3 blendAdd(vec3 base, vec3 blend) {
    return min(base + blend, vec3(1.0));
}

void main() {
    vec3 color = vec3(0.0);
    float iterations = 3.0;

    for(float i = 0.0; i < iterations; i++) {
        float progress = i / (iterations - 1.0);
        float intensity = 1.0 - ((vUv.y - progress) * iterations) * 0.5;
        intensity = smoothstep(0.0, 1.0, intensity);

        vec2 uv = vUv;
        uv.y *= 2.0;
        uv.x += uTime * 0.05; // Slow, stable rotation

        vec3 ringColor = mix(uInnerColor, uOuterColor, progress);
        float noiseIntensity = texture(uNoiseTexture, uv).r;
        ringColor = mix(vec3(0.0), ringColor, noiseIntensity * intensity);
        color = blendAdd(color, ringColor);
    }

    float edgesAttenuation = min(inverseLerp(vUv.y, 0.0, 0.02), inverseLerp(vUv.y, 1.0, 0.5));
    color = mix(vec3(0.0), color, edgesAttenuation);

    FragColor = vec4(color, 1.0);
}
