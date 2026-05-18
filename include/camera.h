#pragma once
#include <glm/glm.hpp>
class Camera {
public:
    glm::vec3 position, front, up, right;
    float yaw=-90.f, pitch=0.f, fov=60.f, speed=5.f, sensitivity=0.08f;
    Camera(glm::vec3 pos={0,0,20});
    glm::mat4 viewMatrix() const;
    glm::mat4 projMatrix(float aspect, float near=0.1f, float far=5000.f) const;
    void processKeyboard(int dir, float dt);
    void processMouse(float dx, float dy);
    void processScroll(float dy);
private:
    void updateVectors();
};
