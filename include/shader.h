#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
    GLuint id = 0;
    Shader() = default;
    Shader(const char* vertPath, const char* fragPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& o) noexcept : id(o.id) { o.id = 0; }
    Shader& operator=(Shader&& o) noexcept {
        if (id) glDeleteProgram(id); id = o.id; o.id = 0; return *this;
    }
    void use() const { glUseProgram(id); }
    void setBool (const std::string& n, bool v)             const;
    void setInt  (const std::string& n, int v)              const;
    void setFloat(const std::string& n, float v)            const;
    void setVec2 (const std::string& n, const glm::vec2& v) const;
    void setVec3 (const std::string& n, const glm::vec3& v) const;
    void setVec4 (const std::string& n, const glm::vec4& v) const;
    void setMat3 (const std::string& n, const glm::mat3& v) const;
    void setMat4 (const std::string& n, const glm::mat4& v) const;
private:
    mutable std::unordered_map<std::string, GLint> cache;
    GLint loc(const std::string& n) const;
    static GLuint compile(GLenum type, const std::string& src, const char* label);
    static std::string readFile(const char* path);
};
