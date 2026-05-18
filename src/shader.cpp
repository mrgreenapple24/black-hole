#include "shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertPath, const char* fragPath) {
    auto vs = readFile(vertPath); auto fs = readFile(fragPath);
    GLuint v = compile(GL_VERTEX_SHADER, vs, vertPath);
    GLuint f = compile(GL_FRAGMENT_SHADER, fs, fragPath);
    id = glCreateProgram();
    glAttachShader(id,v); glAttachShader(id,f); glLinkProgram(id);
    GLint ok; glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if(!ok){ char log[1024]; glGetProgramInfoLog(id,1024,nullptr,log); std::cerr<<"[Link]"<<log<<"\n"; }
    glDeleteShader(v); glDeleteShader(f);
}
Shader::~Shader(){ if(id) glDeleteProgram(id); }
GLuint Shader::compile(GLenum type, const std::string& src, const char* label){
    const char* c=src.c_str(); GLuint s=glCreateShader(type);
    glShaderSource(s,1,&c,nullptr); glCompileShader(s);
    GLint ok; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok){ char log[1024]; glGetShaderInfoLog(s,1024,nullptr,log); std::cerr<<"[Shader "<<label<<"]\n"<<log<<"\n"; }
    return s;
}
std::string Shader::readFile(const char* path){
    std::ifstream f(path);
    if(!f.is_open()){ std::cerr<<"[Shader] Cannot open: "<<path<<"\n"; return ""; }
    std::ostringstream ss; ss<<f.rdbuf(); return ss.str();
}
GLint Shader::loc(const std::string& n) const {
    auto it=cache.find(n); if(it!=cache.end()) return it->second;
    GLint l=glGetUniformLocation(id,n.c_str()); cache[n]=l; return l;
}
void Shader::setBool (const std::string& n, bool v)             const { glUniform1i(loc(n),(int)v); }
void Shader::setInt  (const std::string& n, int v)              const { glUniform1i(loc(n),v); }
void Shader::setFloat(const std::string& n, float v)            const { glUniform1f(loc(n),v); }
void Shader::setVec2 (const std::string& n, const glm::vec2& v) const { glUniform2fv(loc(n),1,glm::value_ptr(v)); }
void Shader::setVec3 (const std::string& n, const glm::vec3& v) const { glUniform3fv(loc(n),1,glm::value_ptr(v)); }
void Shader::setVec4 (const std::string& n, const glm::vec4& v) const { glUniform4fv(loc(n),1,glm::value_ptr(v)); }
void Shader::setMat3 (const std::string& n, const glm::mat3& v) const { glUniformMatrix3fv(loc(n),1,GL_FALSE,glm::value_ptr(v)); }
void Shader::setMat4 (const std::string& n, const glm::mat4& v) const { glUniformMatrix4fv(loc(n),1,GL_FALSE,glm::value_ptr(v)); }
