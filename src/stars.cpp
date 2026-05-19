#include "stars.h"
#include <random>
#include <glm/gtc/constants.hpp>

Stars::Stars(int count) : count(count) {
    std::vector<StarAttr> data(count);
    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);

    for(int i = 0; i < count; ++i) {
        float theta = glm::two_pi<float>() * dist(rng);
        float phi = acosf(2.0f * dist(rng) - 1.0f);
        float r = 400.0f;
        data[i].pos = glm::vec3(r * sinf(phi) * cosf(theta), r * sinf(phi) * sinf(theta), r * cosf(phi));
        data[i].size = dist(rng);
        
        // Random star colors (mostly white, some blue/yellow/red tints)
        float hue = dist(rng);
        if (hue < 0.1f) data[i].color = glm::vec3(1.0f, 0.8f, 0.8f); // red-ish
        else if (hue < 0.2f) data[i].color = glm::vec3(0.8f, 0.8f, 1.0f); // blue-ish
        else if (hue < 0.3f) data[i].color = glm::vec3(1.0f, 1.0f, 0.8f); // yellow-ish
        else data[i].color = glm::vec3(1.0f, 1.0f, 1.0f); // white
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(StarAttr), data.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StarAttr), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(StarAttr), (void*)12);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(StarAttr), (void*)16);
    glBindVertexArray(0);
}

Stars::~Stars() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Stars::draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, float viewHeight) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", proj);
    shader.setFloat("uViewHeight", viewHeight);
    shader.setFloat("uSize", 0.001f);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, count);
    glBindVertexArray(0);
}
