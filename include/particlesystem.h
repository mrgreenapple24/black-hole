#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include "shader.h"

class ParticleSystem {
public:
    ParticleSystem(int count);
    ~ParticleSystem();
    void draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, float time, float viewHeight);

private:
    int count;
    GLuint VAO, VBO;
    struct ParticleAttr {
        float distance;
        float size;
        float random;
    };
};
