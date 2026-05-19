#include "blackholerenderer.h"
#include <glm/gtc/type_ptr.hpp>

BlackHoleRenderer::BlackHoleRenderer(int width, int height)
    : blackHole(),
      particleSystem(60000),
      postProcessor(width, height),
      progBG("shaders/scene.vert", "shaders/scene.frag"),
      progPart("shaders/particle.vert", "shaders/particle.frag"),
      progDist("shaders/scene.vert", "shaders/distort.frag"),
      progFinal("shaders/scene.vert", "shaders/composite.frag"),
      progBlur("shaders/scene.vert", "shaders/blur.frag"),
      progBright("shaders/scene.vert", "shaders/bright.frag") {
}

void BlackHoleRenderer::update(float dt) {
    blackHole.update(dt);
    particleSystem.update(dt, blackHole.position, blackHole.rs());
}

void BlackHoleRenderer::render(Camera &camera, float time) {
    glm::mat4 view = camera.viewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)postProcessor.fboScene.w / postProcessor.fboScene.h, 0.01f, 100.0f);
    glm::mat3 camRot = glm::mat3(glm::inverse(view));

    glm::vec4 bhNDC = proj * view * glm::vec4(blackHole.position, 1.0f);
    glm::vec2 bhScreen;
    if (bhNDC.w > 0.001f) {
        bhScreen = glm::vec2(0.5f + 0.5f * bhNDC.x / bhNDC.w, 0.5f + 0.5f * bhNDC.y / bhNDC.w);
    } else {
        bhScreen = glm::vec2(-10.0f); // Move off-screen
    }

    postProcessor.startFrame();
    renderScene(camera, time);
    // particleSystem.draw(progPart, view, proj, camera.position);
    postProcessor.endFrame();

    postProcessor.renderBloom(progBright, progBlur);
    postProcessor.renderDistortion(progDist, camera.position, camRot, bhScreen);
    postProcessor.renderFinal(progFinal, 0.012f, time);
}

void BlackHoleRenderer::handleResize(int width, int height) {
    postProcessor.resize(width, height);
}

void BlackHoleRenderer::renderScene(Camera &camera, float time) {
    glDepthMask(GL_FALSE);
    progBG.use();
    progBG.setVec2("uResolution", glm::vec2(postProcessor.fboScene.w, postProcessor.fboScene.h));
    progBG.setFloat("uTime", time);
    progBG.setVec3("uCamPos", camera.position);
    progBG.setMat4("uInvView", glm::inverse(camera.viewMatrix()));
    progBG.setVec3("uBHPos", blackHole.position);
    progBG.setFloat("uRS", blackHole.rs());
    
    postProcessor.drawQuad();
    glDepthMask(GL_TRUE);
}
