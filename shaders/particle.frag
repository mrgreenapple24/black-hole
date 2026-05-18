#version 330 core
in vec4 PartColor;
out vec4 FragColor;

void main() {
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    if (dist > 0.5) discard;
    
    float alpha = 1.0 - (dist * 2.0);
    alpha = pow(alpha, 1.5);
    
    FragColor = vec4(PartColor.rgb, PartColor.a * alpha);
}