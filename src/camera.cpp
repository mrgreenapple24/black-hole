#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
Camera::Camera(glm::vec3 pos):position(pos),front(0,0,-1),up(0,1,0),right(1,0,0),target(0,0,0){ 
    distance = glm::length(pos - target);
    // Calculate initial yaw/pitch from position if needed, but for now we'll stick to defaults
    updateVectors(); 
}
glm::mat4 Camera::viewMatrix() const { return glm::lookAt(position,position+front,up); }
glm::mat4 Camera::projMatrix(float a,float n,float f) const { return glm::perspective(glm::radians(fov),a,n,f); }
void Camera::processKeyboard(int d,float dt){
    // WASD movement removed as per request
}
void Camera::processMouse(float dx,float dy){
    yaw+=dx*sensitivity; pitch-=dy*sensitivity;
    pitch=std::clamp(pitch,-89.f,89.f); updateVectors();
}
void Camera::processScroll(float dy){ 
    distance -= dy * 1.0f;
    if (distance < 1.0f) distance = 1.0f;
    updateVectors();
}
void Camera::updateVectors(){
    float yr=glm::radians(yaw),pr=glm::radians(pitch);
    front=glm::normalize(glm::vec3(cos(pr)*cos(yr),sin(pr),cos(pr)*sin(yr)));
    position = target - front * distance;
    right=glm::normalize(glm::cross(front,glm::vec3(0,1,0)));
    up=glm::normalize(glm::cross(right,front));
}
