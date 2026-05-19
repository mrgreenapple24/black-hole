#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

void main() {
    vec3 lightPos = vec3(0, 0, 0); // Black hole light
    vec3 lightColor = vec3(1.0, 0.8, 0.6);
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 ambient = vec3(0.05, 0.05, 0.1);
    FragColor = vec4(ambient + diffuse, 1.0);
}
