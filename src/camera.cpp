#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
Camera::Camera(glm::vec3 pos):position(pos),front(0,0,-1),up(0,1,0),right(1,0,0){ updateVectors(); }
glm::mat4 Camera::viewMatrix() const { return glm::lookAt(position,position+front,up); }
glm::mat4 Camera::projMatrix(float a,float n,float f) const { return glm::perspective(glm::radians(fov),a,n,f); }
void Camera::processKeyboard(int d,float dt){
    float v=speed*dt;
    if(d==0)position+=front*v; if(d==1)position-=front*v;
    if(d==2)position-=right*v; if(d==3)position+=right*v;
    if(d==4)position+=up*v;    if(d==5)position-=up*v;
}
void Camera::processMouse(float dx,float dy){
    yaw+=dx*sensitivity; pitch-=dy*sensitivity;
    pitch=std::clamp(pitch,-89.f,89.f); updateVectors();
}
void Camera::processScroll(float dy){ fov=std::clamp(fov-dy*2.f,10.f,90.f); }
void Camera::updateVectors(){
    float yr=glm::radians(yaw),pr=glm::radians(pitch);
    front=glm::normalize(glm::vec3(cos(pr)*cos(yr),sin(pr),cos(pr)*sin(yr)));
    right=glm::normalize(glm::cross(front,glm::vec3(0,1,0)));
    up=glm::normalize(glm::cross(right,front));
}
