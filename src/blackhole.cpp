#include "blackhole.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
#include <vector>
#include <cmath>

BlackHole::BlackHole(){ buildDisk(); buildHorizon(); buildJets(); }
BlackHole::~BlackHole(){
    if(diskVAO){ glDeleteVertexArrays(1,&diskVAO); glDeleteBuffers(1,&diskVBO); glDeleteBuffers(1,&diskEBO); }
    if(horizonVAO){ glDeleteVertexArrays(1,&horizonVAO); glDeleteBuffers(1,&horizonVBO); glDeleteBuffers(1,&horizonEBO); }
    if(jetVAO){ glDeleteVertexArrays(1,&jetVAO); glDeleteBuffers(1,&jetVBO); }
}
void BlackHole::update(float dt){ time+=dt; spinAngle+=dt*10.f; }

void BlackHole::buildDisk(){
    const int RINGS=160, SEGS=200;
    float inner=rs()*2.0f, outer=rs()*14.0f;
    struct V{ glm::vec3 pos; glm::vec2 uv; glm::vec3 norm; };
    std::vector<V> verts; std::vector<GLuint> idx;
    for(int r=0;r<=RINGS;++r){
        float t=(float)r/RINGS, rad=glm::mix(inner,outer,t);
        for(int s=0;s<=SEGS;++s){
            float phi=glm::two_pi<float>()*s/SEGS;
            verts.push_back({{rad*cosf(phi),0.f,rad*sinf(phi)},{t,(float)s/SEGS},{0,1,0}});
        }
    }
    for(int r=0;r<RINGS;++r) for(int s=0;s<SEGS;++s){
        GLuint a=r*(SEGS+1)+s,b=a+1,c=a+(SEGS+1),d=c+1;
        idx.insert(idx.end(),{a,c,b,b,c,d});
    }
    diskIdxCount=(int)idx.size();
    glGenVertexArrays(1,&diskVAO); glGenBuffers(1,&diskVBO); glGenBuffers(1,&diskEBO);
    glBindVertexArray(diskVAO);
    glBindBuffer(GL_ARRAY_BUFFER,diskVBO); glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(V),verts.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,diskEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*4,idx.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(V),(void*)12);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(V),(void*)20);
    glBindVertexArray(0);
}

void BlackHole::buildHorizon(){
    const int SL=64,SE=64;
    struct V{ glm::vec3 pos; glm::vec3 norm; };
    std::vector<V> verts; std::vector<GLuint> idx;
    for(int i=0;i<=SL;++i){ float phi=glm::pi<float>()*i/SL;
        for(int j=0;j<=SE;++j){ float th=glm::two_pi<float>()*j/SE;
            glm::vec3 n{sinf(phi)*cosf(th),cosf(phi),sinf(phi)*sinf(th)};
            verts.push_back({n,n});
        }
    }
    for(int i=0;i<SL;++i) for(int j=0;j<SE;++j){
        GLuint a=i*(SE+1)+j,b=a+1,c=a+(SE+1),d=c+1;
        idx.insert(idx.end(),{a,b,c,b,d,c});
    }
    horizonIdxCount=(int)idx.size();
    glGenVertexArrays(1,&horizonVAO); glGenBuffers(1,&horizonVBO); glGenBuffers(1,&horizonEBO);
    glBindVertexArray(horizonVAO);
    glBindBuffer(GL_ARRAY_BUFFER,horizonVBO); glBufferData(GL_ARRAY_BUFFER,verts.size()*sizeof(V),verts.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,horizonEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER,idx.size()*4,idx.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),(void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),(void*)12);
    glBindVertexArray(0);
}

void BlackHole::buildJets(){
    const int S=64; float H=rs()*40.f, R=rs()*0.4f;
    std::vector<glm::vec3> verts;
    auto addCone=[&](float dir){
        for(int i=0;i<=S;++i){
            float phi=glm::two_pi<float>()*i/S, t=(float)i/S;
            verts.push_back({R*cosf(phi),dir*rs(),R*sinf(phi)});
            verts.push_back({0.f,dir*(rs()+H*t),0.f});
        }
    };
    addCone(+1.f); addCone(-1.f);
    jetVtxCount=(int)verts.size();
    glGenVertexArrays(1,&jetVAO); glGenBuffers(1,&jetVBO);
    glBindVertexArray(jetVAO);
    glBindBuffer(GL_ARRAY_BUFFER,jetVBO); glBufferData(GL_ARRAY_BUFFER,verts.size()*12,verts.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,12,(void*)0);
    glBindVertexArray(0);
}

void BlackHole::draw(const Shader& disk, const Shader& horizon,
                     const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cam){
    glm::mat4 model=glm::translate(glm::mat4(1.f),position);
    glm::mat4 dm=glm::rotate(model,glm::radians(spinAngle),glm::vec3(0,1,0));

    glDisable(GL_CULL_FACE);
    disk.use();
    disk.setMat4("model",dm); disk.setMat4("view",view); disk.setMat4("projection",proj);
    disk.setVec3("camPos",cam); disk.setFloat("time",time); disk.setFloat("rs",rs());
    disk.setVec3("bhPos",position); disk.setBool("isJet",false);
    glBindVertexArray(diskVAO); glDrawElements(GL_TRIANGLES,diskIdxCount,GL_UNSIGNED_INT,nullptr);

    disk.setMat4("model",model); disk.setBool("isJet",true);
    glBindVertexArray(jetVAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,jetVtxCount/2);
    glDrawArrays(GL_TRIANGLE_STRIP,jetVtxCount/2,jetVtxCount/2);
    glBindVertexArray(0);
    disk.setBool("isJet",false);
    glEnable(GL_CULL_FACE);

    horizon.use();
    glm::mat4 hm=glm::scale(model,glm::vec3(rs()));
    horizon.setMat4("model",hm); horizon.setMat4("view",view); horizon.setMat4("projection",proj);
    horizon.setVec3("camPos",cam); horizon.setFloat("time",time);
    glBindVertexArray(horizonVAO); glDrawElements(GL_TRIANGLES,horizonIdxCount,GL_UNSIGNED_INT,nullptr);
    glBindVertexArray(0);
}
