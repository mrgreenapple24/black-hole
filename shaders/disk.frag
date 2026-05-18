#version 330 core
in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;
in vec3 LocalPos;

out vec4 FragColor;

uniform vec3 camPos;
uniform vec3 bhPos;
uniform float time;
uniform float rs;
uniform bool isJet;

float hash(vec2 p) {
    return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x))));
}

float noise(vec2 x) {
    vec2 i = floor(x);
    vec2 f = fract(x);
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.5));
    for (int i = 0; i < 5; ++i) {
        v += a * noise(p);
        p = rot * p * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

void main() {
    if (isJet) {
        float dist = length(LocalPos);
        float intensity = 1.0 / (1.0 + dist * 0.1);
        intensity = pow(intensity, 3.0);
        
        vec3 color = vec3(0.5, 0.7, 1.0) * intensity * 2.0;
        float alpha = intensity * 0.8;
        FragColor = vec4(color, alpha);
        return;
    }

    float distFromCenter = length(LocalPos);
    vec2 uv = vec2(TexCoords.x * 5.0, TexCoords.y * 20.0 - time * 2.0);
    float n = fbm(uv + fbm(uv * 2.0 + time));
    
    vec3 viewDir = normalize(camPos - FragPos);
    vec3 tangent = normalize(cross(vec3(0,1,0), LocalPos));
    float doppler = dot(tangent, viewDir);
    
    float temp = 1.0 - TexCoords.x;
    vec3 hotColor = vec3(1.0, 0.9, 0.6);
    vec3 coolColor = vec3(0.8, 0.3, 0.0);
    vec3 baseColor = mix(coolColor, hotColor, temp);
    
    vec3 dopplerColor = baseColor;
    if (doppler > 0.0) {
        dopplerColor = mix(baseColor, vec3(0.5, 0.8, 1.0), doppler * 0.8);
    } else {
        dopplerColor = mix(baseColor, vec3(1.0, 0.1, 0.0), -doppler * 0.8);
    }
    
    float intensity = pow(n, 1.5) * (1.0 - TexCoords.x) * 2.0;
    float edgeFade = smoothstep(0.0, 0.1, TexCoords.x) * smoothstep(1.0, 0.8, TexCoords.x);
    vec3 finalColor = dopplerColor * intensity * 3.0;
    
    FragColor = vec4(finalColor, edgeFade * n);
}