#ifndef STARS_H
#define STARS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"

class Stars {
public:
    Stars(int count);
    ~Stars();
    void draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, float viewHeight);

private:
    int count;
    GLuint VAO, VBO;
    struct StarAttr {
        glm::vec3 pos;
        float size;
        glm::vec3 color;
    };
};

#endif
