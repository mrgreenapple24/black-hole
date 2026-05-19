#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform sampler2D uScene;
uniform sampler2D uDistortion;
uniform sampler2D uBloom;
uniform float     uAberration;
uniform float     uTime;

float vignette(vec2 uv){
    vec2 d = uv - 0.5;
    return 1.0 - dot(d,d) * 1.2; // Slightly softer vignette
}

void main(){
    vec2 uv = vUV;
    
    // Read distortion map
    vec2 distort = (texture(uDistortion, uv).rg - 0.5) * 2.0;
    
    // RGB chromatic aberration
    float ab = uAberration;
    vec2 uvR = uv + distort * (1.0 + ab);
    vec2 uvG = uv + distort;
    vec2 uvB = uv + distort * (1.0 - ab);
    
    float r = texture(uScene, uvR).r;
    float g = texture(uScene, uvG).g;
    float b = texture(uScene, uvB).b;
    vec3 col = vec3(r, g, b);
    
    // Bloom (Reduced)
    col += texture(uBloom, uv).rgb * 0.4;
    
    // Safety clamp before tone mapping
    col = max(vec3(0.0), col);

    // Simple ACES-like tone mapping
    float a = 2.51;
    float b = 0.03;
    float c = 2.43;
    float d = 0.59;
    float e = 0.14;
    col = (col * (a * col + b)) / (col * (c * col + d) + e);
    
    // Gamma correction
    col = pow(col, vec3(1.0/2.2));
    
    // Vignette
    col *= vignette(uv);
    
    // Final hard clamp
    fragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
