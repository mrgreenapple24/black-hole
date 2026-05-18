#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;
uniform vec3 camPos;

void main() {
    vec3 viewDir = normalize(camPos - FragPos);
    // Fresnel-like edge glow
    float edge = 1.0 - max(dot(normalize(Normal), viewDir), 0.0);
    edge = pow(edge, 15.0); // Sharper but fainter
    
    // Ghostly violet/white glow
    vec3 glowColor = vec3(0.7, 0.8, 1.0) * edge * 0.3;
    FragColor = vec4(glowColor, 1.0);
}