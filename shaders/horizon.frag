#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;
uniform vec3 camPos;

void main() {
    vec3 viewDir = normalize(camPos - FragPos);
    float edge = 1.0 - max(dot(normalize(Normal), viewDir), 0.0);
    edge = pow(edge, 10.0);
    
    vec3 ringColor = vec3(1.0, 0.6, 0.1) * edge * 0.5;
    FragColor = vec4(ringColor, 1.0);
}