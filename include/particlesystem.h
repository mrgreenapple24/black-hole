#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include <vector>
#include <random>

struct Particle {
    glm::vec3 pos, vel;
    glm::vec4 color;
    float life, maxLife, size;
    float orbitAngle, orbitRadius, orbitY, angularVel;
};

class ParticleSystem {
public:
    ParticleSystem(int maxParticles = 60000);
    ~ParticleSystem();
    void update(float dt, const glm::vec3& bhPos, float rs);
    void draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos);
    int activeCount() const { return (int)particles.size(); }
private:
    std::vector<Particle> particles;
    int maxCount;
    GLuint VAO=0, VBO=0;
    std::mt19937 rng{42};
    struct GPUParticle{ glm::vec3 pos; float size; glm::vec4 color; };
    std::vector<GPUParticle> gpuBuf;
    void spawnParticle(const glm::vec3& bhPos, float rs);
    void uploadToGPU();
};
