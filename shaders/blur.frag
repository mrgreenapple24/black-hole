#version 330 core
in  vec2 vUV;
out vec4 fragColor;
uniform sampler2D uTex;
uniform vec2      uDir;
void main(){
    float weights[7] = float[](0.0044,0.054,0.242,0.398,0.242,0.054,0.0044);
    vec3 col = vec3(0.0);
    vec2 d = uDir;
    for(int i=0;i<7;i++){
        col += texture(uTex, vUV + d*(float(i)-3.0)).rgb * weights[i];
    }
    fragColor = vec4(col, 1.0);
}
