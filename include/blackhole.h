#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class BlackHole {
public:
    glm::vec3 position = {0,0,0};
    float mass = 1.0f;
    float spinAngle = 0.0f;
    float time = 0.0f;

    BlackHole();
    ~BlackHole();
    void update(float dt);
    void draw(const Shader& diskShader, const Shader& horizonShader,
              const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos);
    float rs() const { return 1.5f * mass; }

private:
    GLuint diskVAO=0,diskVBO=0,diskEBO=0; int diskIdxCount=0;
    GLuint horizonVAO=0,horizonVBO=0,horizonEBO=0; int horizonIdxCount=0;
    GLuint jetVAO=0,jetVBO=0; int jetVtxCount=0;
    void buildDisk(); void buildHorizon(); void buildJets();
};
