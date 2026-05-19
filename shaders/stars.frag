#version 330 core
out vec4 FragColor;
in vec3 vColor;

void main() {
    float distanceToCenter = length(gl_PointCoord - vec2(0.5));
    if(distanceToCenter > 0.5)
        discard;
    FragColor = vec4(vColor, 1.0);
}
