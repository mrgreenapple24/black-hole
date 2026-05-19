#include "particlesystem.h"
#include <glm/gtc/matrix_transform.hpp>

ParticleSystem::ParticleSystem(int count) : count(count) {
    std::vector<ParticleAttr> data(count);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(int i = 0; i < count; ++i) {
        if (i < count / 2) {
            // Accretion disk particles
            data[i].distance = dist(rng);
            data[i].size = dist(rng);
            data[i].random = dist(rng);
        } else {
            // 3D cloud particles
            // Use distance > 1.0 to avoid being inside the horizon
            data[i].distance = 1.0f + dist(rng) * 10.0f;
            data[i].size = dist(rng) * 0.5f; // Slightly smaller
            data[i].random = dist(rng) + 10.0f; // Offset random to distinguish in shader
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(ParticleAttr), data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttr), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttr), (void*)4);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleAttr), (void*)8);
    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void ParticleSystem::draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, float time, float viewHeight) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);
    shader.setFloat("uTime", time);
    shader.setVec3("uInnerColor", glm::vec3(1.0f, 0.5f, 0.5f));
    shader.setVec3("uOuterColor", glm::vec3(0.21f, 0.2f, 1.0f));
    shader.setFloat("uViewHeight", viewHeight);
    shader.setFloat("uSize", 0.015f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, count);
    glBindVertexArray(0);
}
