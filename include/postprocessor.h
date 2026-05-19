#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

struct FBO {
    GLuint fbo, tex;
    int w, h;

    FBO() : fbo(0), tex(0), w(0), h(0) {}
    ~FBO();
    void create(int width, int height, GLint internalFmt = GL_RGB, GLenum fmt = GL_RGB, GLenum type = GL_UNSIGNED_BYTE);
    void bind();
    void unbind();
};

class PostProcessor {
public:
    PostProcessor(int width, int height);
    ~PostProcessor();
    void resize(int width, int height);
    
    void startSpacePass();
    void startDistortionPass();
    void renderFinal(Shader &shader, const glm::vec2 &bhScreenPos, float rgbShift);
    
    void drawQuad();

    FBO fboSpace, fboDistortion;

private:
    int width, height;
    GLuint quadVAO, quadVBO;
    void initQuad();
};

#endif
