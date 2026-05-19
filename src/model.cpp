#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "model.h"
#include <iostream>

Model::Model(const std::string& path) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
    if (!warn.empty()) std::cout << "GLTF Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "GLTF Error: " << err << std::endl;
    if (!ret) return;

    for (const auto& mesh : model.meshes) loadMesh(mesh);
}

Model::~Model() {
    for (auto& m : meshes) {
        glDeleteVertexArrays(1, &m.vao);
        for (auto vbo : m.vbos) glDeleteBuffers(1, &vbo);
    }
}

void Model::loadMesh(const tinygltf::Mesh& mesh) {
    for (const auto& primitive : mesh.primitives) {
        Mesh m;
        glGenVertexArrays(1, &m.vao);
        glBindVertexArray(m.vao);

        for (auto const& [name, accessorIdx] : primitive.attributes) {
            const tinygltf::Accessor& accessor = model.accessors[accessorIdx];
            const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);

            int loc = -1;
            if (name == "POSITION") loc = 0;
            else if (name == "NORMAL") loc = 1;
            else if (name == "TEXCOORD_0") loc = 2;

            if (loc != -1) {
                glEnableVertexAttribArray(loc);
                glVertexAttribPointer(loc, accessor.type == TINYGLTF_TYPE_VEC3 ? 3 : 2, accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE, bufferView.byteStride == 0 ? (accessor.type == TINYGLTF_TYPE_VEC3 ? 12 : 8) : (int)bufferView.byteStride, (void*)(size_t)accessor.byteOffset);
            }
            m.vbos.push_back(vbo);
        }

        if (primitive.indices >= 0) {
            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& bufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

            GLuint ebo;
            glGenBuffers(1, &ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferView.byteLength, &buffer.data[bufferView.byteOffset], GL_STATIC_DRAW);
            m.vbos.push_back(ebo);
            m.indexCount = (int)indexAccessor.count;
            m.indexType = indexAccessor.componentType;
        }
        m.mode = primitive.mode;
        meshes.push_back(m);
    }
}

void Model::draw(const Shader& shader, const glm::mat4& modelMat, const glm::mat4& view, const glm::mat4& projection) {
    shader.use();
    shader.setMat4("model", modelMat);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    for (const auto& m : meshes) {
        glBindVertexArray(m.vao);
        if (m.indexCount > 0) glDrawElements(m.mode, m.indexCount, m.indexType, 0);
        else glDrawArrays(m.mode, 0, m.indexCount);
    }
    glBindVertexArray(0);
}
