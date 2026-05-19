#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform sampler2D uTex;
uniform float     uThreshold;
void main(){
    vec3 col = texture(uTex, vUV).rgb;
    float lum = dot(col, vec3(0.2126,0.7152,0.0722));
    float mask = clamp((lum - uThreshold)*4.0, 0.0, 1.0);
    fragColor = vec4(col * mask, 1.0);
}
