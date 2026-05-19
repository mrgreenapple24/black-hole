#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class BlackHole {
public:
    glm::vec3 position = {0,0,0};
    float time = 0.0f;
    GLuint noiseTexture = 0;

    BlackHole();
    ~BlackHole();
    void update(float dt);
    
    void drawSpace(const Shader& diskShader, const glm::mat4& view, const glm::mat4& proj);
    void drawDistortion(const Shader& activeShader, const Shader& maskShader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos);

private:
    GLuint diskVAO=0, diskVBO=0, diskEBO=0; int diskIdxCount=0;
    GLuint distActiveVAO=0, distActiveVBO=0;
    GLuint distMaskVAO=0, distMaskVBO=0;

    void buildDisk();
    void buildDistortionMeshes();
};
