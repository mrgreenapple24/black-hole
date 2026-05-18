#include "particlesystem.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

ParticleSystem::ParticleSystem(int max) : maxCount(max) {
    particles.reserve(max); gpuBuf.reserve(max);
    glGenVertexArrays(1,&VAO); glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,max*sizeof(GPUParticle),nullptr,GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GPUParticle),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,1,GL_FLOAT,GL_FALSE,sizeof(GPUParticle),(void*)12);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,sizeof(GPUParticle),(void*)16);
    glBindVertexArray(0);
}
ParticleSystem::~ParticleSystem(){ glDeleteVertexArrays(1,&VAO); glDeleteBuffers(1,&VBO); }

void ParticleSystem::spawnParticle(const glm::vec3& bhPos, float rs){
    std::uniform_real_distribution<float> aD(0,glm::two_pi<float>());
    std::uniform_real_distribution<float> rD(rs*2.2f,rs*15.f);
    std::uniform_real_distribution<float> yD(-rs*0.25f,rs*0.25f);
    std::uniform_real_distribution<float> lD(4.f,16.f);
    std::uniform_real_distribution<float> sD(0.02f,0.18f);
    Particle p;
    p.orbitAngle=aD(rng); p.orbitRadius=rD(rng); p.orbitY=yD(rng);
    p.angularVel=1.8f/sqrtf(p.orbitRadius/rs);
    p.maxLife=lD(rng); p.life=p.maxLife; p.size=sD(rng);
    p.pos=bhPos+glm::vec3(p.orbitRadius*cosf(p.orbitAngle),p.orbitY,p.orbitRadius*sinf(p.orbitAngle));
    float heat=1.f-((p.orbitRadius-rs*2.2f)/(rs*12.8f));
    heat=glm::clamp(heat,0.f,1.f);
    p.color={glm::mix(0.8f,1.f,heat),glm::mix(0.05f,0.7f,heat*heat),glm::mix(0.f,0.35f,heat*heat*heat),glm::mix(0.3f,1.f,heat)};
    particles.push_back(p);
}

void ParticleSystem::update(float dt, const glm::vec3& bhPos, float rs){
    int deficit=maxCount-(int)particles.size();
    for(int i=0;i<std::min(deficit,150);++i) spawnParticle(bhPos,rs);
    for(auto& p:particles){
        p.life-=dt;
        float lr=p.life/p.maxLife, ins=1.f-lr*lr;
        p.orbitRadius-=ins*dt*rs*0.35f;
        p.orbitRadius=std::max(p.orbitRadius,rs*1.05f);
        p.angularVel=1.8f/sqrtf(p.orbitRadius/rs);
        p.orbitAngle+=p.angularVel*dt;
        float vx=-sinf(p.orbitAngle);
        float doppler=(vx+1.f)*0.5f;
        p.pos=bhPos+glm::vec3(p.orbitRadius*cosf(p.orbitAngle),p.orbitY,p.orbitRadius*sinf(p.orbitAngle));
        float prox=glm::clamp((p.orbitRadius-rs)/(rs*3.f),0.f,1.f);
        p.color.a=lr*prox;
        // Doppler shift: blue on approaching, red on receding
        p.color.r=glm::clamp(glm::mix(p.color.r*0.5f,p.color.r*1.4f,doppler),0.f,3.f);
        p.color.b=glm::clamp(glm::mix(p.color.b*1.6f,p.color.b*0.2f,doppler),0.f,3.f);
    }
    particles.erase(std::remove_if(particles.begin(),particles.end(),
        [&](const Particle& p){ return p.life<=0.f||p.orbitRadius<=rs*1.02f; }),particles.end());
}

void ParticleSystem::uploadToGPU(){
    gpuBuf.clear(); gpuBuf.reserve(particles.size());
    for(auto& p:particles) gpuBuf.push_back({p.pos,p.size,p.color});
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,gpuBuf.size()*sizeof(GPUParticle),gpuBuf.data());
}

void ParticleSystem::draw(const Shader& shader, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cam){
    if(particles.empty()) return;
    uploadToGPU();
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE); glDepthMask(GL_FALSE);
    shader.use();
    shader.setMat4("view",view); shader.setMat4("projection",proj); shader.setVec3("camPos",cam);
    glBindVertexArray(VAO); glDrawArrays(GL_POINTS,0,(GLsizei)gpuBuf.size());
    glBindVertexArray(0);
    glDepthMask(GL_TRUE); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}
