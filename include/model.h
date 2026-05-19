#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <tiny_gltf.h>
#include "shader.h"

class Model {
public:
    Model(const std::string& path);
    ~Model();
    void draw(const Shader& shader, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

private:
    tinygltf::Model model;
    struct Mesh {
        GLuint vao;
        std::vector<GLuint> vbos;
        int indexCount;
        GLenum indexType;
        int mode;
    };
    std::vector<Mesh> meshes;

    void loadNode(int nodeIdx);
    void loadMesh(const tinygltf::Mesh& mesh);
};

#endif
