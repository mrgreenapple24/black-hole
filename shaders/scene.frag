#version 330 core
in  vec2 vUV;
out vec4 fragColor;

uniform vec2  uResolution;
uniform float uTime;
uniform vec3  uCamPos;
uniform mat4  uInvView;
uniform vec3  uBHPos;
uniform float uRS;

// --- Constants ---
#define PI 3.14159265359
#define MAX_STEPS 64
#define STEP_SIZE 0.4

// --- Noise / Utilities ---
float hash(float n) { return fract(sin(n) * 43758.5453123); }
float noise(vec3 x) {
    vec3 p = floor(x); vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + p.z*113.0;
    return mix(mix(mix(hash(n+  0.0), hash(n+  1.0), f.x),
                   mix(hash(n+ 57.0), hash(n+ 58.0), f.x), f.y),
               mix(mix(hash(n+113.0), hash(n+114.0), f.x),
                   mix(hash(n+170.0), hash(n+171.0), f.x), f.y), f.z);
}

vec3 stars(vec3 dir) {
    float n = noise(dir * 100.0);
    float star = pow(n, 20.0) * 10.0;
    return vec3(star) * mix(vec3(0.8,0.9,1.0), vec3(1.0,0.8,0.6), hash(dir.x));
}

// --- Accretion Disk Sampling ---
vec3 getDiskColor(vec3 p) {
    float r = length(p.xz);
    float inner = uRS * 2.5;
    float outer = uRS * 12.0;
    if (r < inner || r > outer) return vec3(0.0);
    
    // Texture/Noise for the disk
    float angle = atan(p.z, p.x);
    float distNorm = (r - inner) / (outer - inner);
    
    float n = noise(vec3(r * 0.5, angle * 2.0, uTime * 0.2));
    n += 0.5 * noise(vec3(r * 1.2, angle * 4.0, -uTime * 0.4));
    
    vec3 color = mix(vec3(1.0, 0.4, 0.1), vec3(0.2, 0.1, 0.8), distNorm);
    float intensity = pow(n, 2.0) * (1.0 - distNorm) * 2.5;
    
    return color * intensity;
}

void main() {
    vec2 uv = (vUV * 2.0 - 1.0);
    uv.x *= uResolution.x / uResolution.y;

    // Initial Ray
    vec4 rayDirCam = vec4(normalize(vec3(uv, -1.2)), 0.0);
    vec3 rd = normalize((uInvView * rayDirCam).xyz);
    vec3 ro = uCamPos;
    
    vec3 bhPos = uBHPos;
    float rs = max(uRS, 0.1);
    
    vec3 p = ro;
    vec3 v = rd;
    
    vec3 finalCol = vec3(0.0);
    float diskAlpha = 0.0;
    bool hitHorizon = false;

    // Raymarching with simple bending
    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 relP = p - bhPos;
        float r2 = dot(relP, relP);
        float r = sqrt(r2);
        
        if (r < rs * 1.05) {
            hitHorizon = true;
            break;
        }
        
        // Acceleration toward BH (Gravitational Lensing)
        // a = -1.5 * rs * (L^2 / r^5) * relP
        // We'll use a simplified bending force
        float force = (1.5 * rs) / (r2 * r + 1e-4);
        v = normalize(v - relP * force * STEP_SIZE);
        
        // Step forward
        vec3 nextP = p + v * STEP_SIZE;
        
        // Check intersection with the accretion disk plane (y = bhPos.y)
        if ((p.y - bhPos.y) * (nextP.y - bhPos.y) < 0.0) {
            float t = (bhPos.y - p.y) / (nextP.y - p.y);
            vec3 intersectP = p + v * t * STEP_SIZE;
            vec3 diskCol = getDiskColor(intersectP);
            if (length(diskCol) > 0.0) {
                finalCol += diskCol * (1.0 - diskAlpha);
                diskAlpha += 0.4; // Semi-transparent disk
                if (diskAlpha >= 1.0) break;
            }
        }
        
        p = nextP;
        
        // Break if we're moving away and far enough
        if (r > 100.0 && dot(v, relP) > 0.0) break;
    }

    if (hitHorizon) {
        // Event Horizon is black
        finalCol = mix(finalCol, vec3(0.0), 1.0 - diskAlpha);
    } else {
        // Star background
        finalCol += stars(v) * (1.0 - diskAlpha);
        
        // Thin Photon Ring
        vec3 relP = p - bhPos;
        float impact = length(cross(rd, ro - bhPos));
        float ring = exp(-pow(abs(impact - rs * 2.6), 2.0) * 10.0);
        finalCol += vec3(1.0, 0.8, 0.6) * ring * 2.0 * (1.0 - diskAlpha);
    }

    // Safety clamp and output
    fragColor = vec4(clamp(finalCol, 0.0, 10.0), 1.0);
}