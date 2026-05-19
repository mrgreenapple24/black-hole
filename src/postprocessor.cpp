#include "postprocessor.h"
#include <iostream>

FBO::~FBO() {
    if (fbo) glDeleteFramebuffers(1, &fbo);
    if (tex) glDeleteTextures(1, &tex);
}

void FBO::create(int width, int height, GLint internalFmt, GLenum fmt, GLenum type) {
    if (fbo) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &tex);
    }
    w = width; h = height;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &tex);
    
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, w, h, 0, fmt, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); glViewport(0, 0, w, h); }
void FBO::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

PostProcessor::PostProcessor(int width, int height) : width(width), height(height), quadVAO(0), quadVBO(0) {
    initQuad();
    resize(width, height);
}

PostProcessor::~PostProcessor() {
    if (quadVAO) glDeleteVertexArrays(1, &quadVAO);
    if (quadVBO) glDeleteBuffers(1, &quadVBO);
}

void PostProcessor::resize(int width, int height) {
    this->width = width; this->height = height;
    fboSpace.create(width * 2, height * 2, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
    fboDistortion.create(width / 2, height / 2, GL_RED, GL_RED, GL_UNSIGNED_BYTE);
}

void PostProcessor::initQuad() {
    float quadVertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
}

void PostProcessor::startSpacePass() {
    fboSpace.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::startDistortionPass() {
    fboDistortion.bind();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessor::renderFinal(Shader &shader, const glm::vec2 &bhScreenPos, float rgbShift) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fboSpace.tex);
    shader.setInt("uSpaceTexture", 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, fboDistortion.tex);
    shader.setInt("uDistortionTexture", 1);
    
    shader.setVec2("uBlackHolePosition", bhScreenPos);
    shader.setFloat("uRGBShiftRadius", rgbShift);
    
    drawQuad();
}

void PostProcessor::drawQuad() {
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
