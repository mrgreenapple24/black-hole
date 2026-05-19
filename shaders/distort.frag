#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform vec2  uResolution;
uniform vec3  uCamPos;
uniform mat3  uCamRot;
uniform vec2  uBHScreen;
void main(){
    vec2 uv = vUV;
    vec2 toCenter = uBHScreen - uv;
    float dist = length(toCenter);
    float strength = 0.02 / (dist * dist * 30.0 + 0.01);
    strength = clamp(strength, 0.0, 0.15);
    vec2 offset = normalize(toCenter + vec2(0.0001)) * strength;
    fragColor = vec4(offset * 0.5 + 0.5, 0.0, 1.0);
}
