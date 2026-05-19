#ifndef NOISES_H
#define NOISES_H

#include <glad/glad.h>
#include "shader.h"

class Noises {
public:
    static GLuint create(int width, int height);
private:
    static void initQuad();
    static GLuint quadVAO, quadVBO;
};

#endif
