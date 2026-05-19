#include "blackhole.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

BlackHole::BlackHole() {
    buildDisk();
    buildHorizon();
    buildDistortionMeshes();
}

BlackHole::~BlackHole() {
    if(diskVAO) { glDeleteVertexArrays(1,&diskVAO); glDeleteBuffers(1,&diskVBO); glDeleteBuffers(1,&diskEBO); }
    if(horizonVAO) { glDeleteVertexArrays(1,&horizonVAO); glDeleteBuffers(1,&horizonVBO); glDeleteBuffers(1,&horizonEBO); }
    if(distActiveVAO) { glDeleteVertexArrays(1,&distActiveVAO); glDeleteBuffers(1,&distActiveVBO); }
    if(distMaskVAO) { glDeleteVertexArrays(1,&distMaskVAO); glDeleteBuffers(1,&distMaskVBO); }
}

void BlackHole::update(float dt) {
    time += dt;
}

void BlackHole::buildDisk() {
    const int RINGS = 64, SEGS = 64;
    float inner = 1.0f, outer = 5.0f;
    struct V { glm::vec3 pos; glm::vec2 uv; };
    std::vector<V> verts; std::vector<GLuint> idx;
    for(int r = 0; r <= RINGS; ++r) {
        float t = (float)r / RINGS, rad = glm::mix(inner, outer, t);
        for(int s = 0; s <= SEGS; ++s) {
            float phi = glm::two_pi<float>() * s / SEGS;
            verts.push_back({{rad * cosf(phi), 0.0f, rad * sinf(phi)}, {(float)s / SEGS, t}});
        }
    }
    for(int r = 0; r < RINGS; ++r) for(int s = 0; s < SEGS; ++s) {
        GLuint a = r * (SEGS + 1) + s, b = a + 1, c = a + (SEGS + 1), d = c + 1;
        idx.insert(idx.end(), {a, c, b, b, c, d});
    }
    diskIdxCount = (int)idx.size();
    glGenVertexArrays(1, &diskVAO); glGenBuffers(1, &diskVBO); glGenBuffers(1, &diskEBO);
    glBindVertexArray(diskVAO);
    glBindBuffer(GL_ARRAY_BUFFER, diskVBO); glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(V), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, diskEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * 4, idx.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(V), (void*)12);
    glBindVertexArray(0);
}

void BlackHole::buildHorizon() {
    const int SL = 32, SE = 32;
    struct V { glm::vec3 pos; };
    std::vector<V> verts; std::vector<GLuint> idx;
    for(int i = 0; i <= SL; ++i) {
        float phi = glm::pi<float>() * i / SL;
        for(int j = 0; j <= SE; ++j) {
            float th = glm::two_pi<float>() * j / SE;
            verts.push_back({{sinf(phi) * cosf(th), cosf(phi), sinf(phi) * sinf(th)}});
        }
    }
    for(int i = 0; i < SL; ++i) for(int j = 0; j < SE; ++j) {
        GLuint a = i * (SE + 1) + j, b = a + 1, c = a + (SE + 1), d = c + 1;
        idx.insert(idx.end(), {a, b, c, b, d, c});
    }
    horizonIdxCount = (int)idx.size();
    glGenVertexArrays(1, &horizonVAO); glGenBuffers(1, &horizonVBO); glGenBuffers(1, &horizonEBO);
    glBindVertexArray(horizonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, horizonVBO); glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(V), verts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, horizonEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * 4, idx.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), (void*)0);
    glBindVertexArray(0);
}

void BlackHole::buildDistortionMeshes() {
    float quad[] = {
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &distActiveVAO); glGenBuffers(1, &distActiveVBO);
    glBindVertexArray(distActiveVAO);
    glBindBuffer(GL_ARRAY_BUFFER, distActiveVBO); glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(12));

    glGenVertexArrays(1, &distMaskVAO); glGenBuffers(1, &distMaskVBO);
    glBindVertexArray(distMaskVAO);
    glBindBuffer(GL_ARRAY_BUFFER, distMaskVBO); glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(12));
    glBindVertexArray(0);
}

void BlackHole::drawSpace(const Shader& diskShader, const Shader& horizonShader, const glm::mat4& view, const glm::mat4& proj) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    
    // Draw Horizon (Black Sphere)
    horizonShader.use();
    glm::mat4 hModel = glm::scale(model, glm::vec3(0.95f)); // Slightly smaller to avoid z-fighting
    horizonShader.setMat4("model", hModel);
    horizonShader.setMat4("view", view);
    horizonShader.setMat4("projection", proj);
    glBindVertexArray(horizonVAO);
    glDrawElements(GL_TRIANGLES, horizonIdxCount, GL_UNSIGNED_INT, 0);

    // Draw Disk
    diskShader.use();
    diskShader.setMat4("model", model);
    diskShader.setMat4("view", view);
    diskShader.setMat4("projection", proj);
    diskShader.setFloat("uTime", time);
    diskShader.setVec3("uInnerColor", glm::vec3(1.0f, 0.5f, 0.5f)); // #ff8080
    diskShader.setVec3("uOuterColor", glm::vec3(0.21f, 0.2f, 1.0f)); // #3633ff
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    diskShader.setInt("uNoiseTexture", 0);

    glBindVertexArray(diskVAO);
    glDrawElements(GL_TRIANGLES, diskIdxCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void BlackHole::drawDistortion(const Shader& activeShader, const Shader& maskShader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& camPos) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    
    // Active mesh (billboarded)
    activeShader.use();
    glm::mat4 activeModel = glm::scale(model, glm::vec3(10.0f));
    // Simple billboard logic: look at camera
    glm::vec3 look = glm::normalize(camPos - position);
    glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0,1,0), look));
    glm::vec3 up = glm::cross(look, right);
    glm::mat4 rot = glm::mat4(1.0f);
    rot[0] = glm::vec4(right, 0.0f);
    rot[1] = glm::vec4(up, 0.0f);
    rot[2] = glm::vec4(look, 0.0f);
    activeModel = activeModel * rot;

    activeShader.setMat4("model", activeModel);
    activeShader.setMat4("view", view);
    activeShader.setMat4("projection", proj);
    glBindVertexArray(distActiveVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Mask mesh (horizontal plane)
    maskShader.use();
    glm::mat4 maskModel = glm::scale(model, glm::vec3(10.0f));
    maskModel = glm::rotate(maskModel, glm::radians(90.0f), glm::vec3(1, 0, 0));
    maskShader.setMat4("model", maskModel);
    maskShader.setMat4("view", view);
    maskShader.setMat4("projection", proj);
    glBindVertexArray(distMaskVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindVertexArray(0);
}
