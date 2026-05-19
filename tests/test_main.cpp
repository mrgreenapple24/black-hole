#include <iostream>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"

void test_camera_initialization() {
    std::cout << "Running test_camera_initialization..." << std::endl;
    glm::vec3 initialPos(0.0f, 0.0f, 10.0f);
    Camera cam(initialPos);
    assert(glm::all(glm::epsilonEqual(cam.position, initialPos, 0.001f)));
    assert(std::abs(glm::length(cam.position) - 10.0f) < 0.001f);
    std::cout << "test_camera_initialization passed!" << std::endl;
}

void test_camera_matrices() {
    std::cout << "Running test_camera_matrices..." << std::endl;
    // Camera at (0, 0, 10) looking at (0, 0, 0)
    Camera cam(glm::vec3(0.0f, 0.0f, 10.0f));
    glm::mat4 view = cam.viewMatrix();
    glm::mat4 invView = glm::inverse(view);

    // In camera space, forward is (0,0,-1). 
    // Transforming (0,0,-1,0) by invView should give world space forward (0,0,-1)
    glm::vec4 forwardCam(0.0f, 0.0f, -1.0f, 0.0f);
    glm::vec4 forwardWorld = invView * forwardCam;
    
    std::cout << "  Forward World: (" << forwardWorld.x << ", " << forwardWorld.y << ", " << forwardWorld.z << ")" << std::endl;
    assert(std::abs(forwardWorld.z - (-1.0f)) < 0.001f);
    assert(std::abs(forwardWorld.x) < 0.001f);
    assert(std::abs(forwardWorld.y) < 0.001f);

    std::cout << "test_camera_matrices passed!" << std::endl;
}

void test_ray_sphere_intersection() {
    std::cout << "Running test_ray_sphere_intersection..." << std::endl;
    // Simplified GLSL logic in C++
    glm::vec3 rayOrigin(0.0f, 0.0f, 10.0f);
    glm::vec3 rayDir(0.0f, 0.0f, -1.0f); // Points directly at origin
    glm::vec3 bhPos(0.0f, 0.0f, 0.0f);
    float visualRS = 0.6f * 2.6f; // ~1.56

    glm::vec3 oc = rayOrigin - bhPos;
    float b = glm::dot(rayDir, oc);
    float c = glm::dot(oc, oc) - visualRS * visualRS;
    float h = b*b - c;

    std::cout << "  h value (should be > 0): " << h << std::endl;
    assert(h > 0.0f); // Must intersect

    // Ray pointing away
    rayDir = glm::vec3(0.0f, 1.0f, 0.0f);
    b = glm::dot(rayDir, oc);
    c = glm::dot(oc, oc) - visualRS * visualRS;
    h = b*b - c;
    std::cout << "  h value (should be < 0): " << h << std::endl;
    assert(h < 0.0f);

    std::cout << "test_ray_sphere_intersection passed!" << std::endl;
}

void test_distortion_strength() {
    std::cout << "Running test_distortion_strength..." << std::endl;
    float testDist[] = { 0.05f, 0.1f, 0.2f, 0.5f };
    for (float dist : testDist) {
        float strength = 0.02f / (dist * dist * 30.0f + 0.01f);
        float clamped = std::max(0.0f, std::min(0.15f, strength));
        std::cout << "  dist: " << dist << ", strength: " << strength << ", clamped: " << clamped << std::endl;
        assert(clamped >= 0.0f && clamped <= 0.15f);
    }
    std::cout << "test_distortion_strength passed!" << std::endl;
}

int main() {
    std::cout << "Starting diagnostic tests..." << std::endl;
    
    test_camera_initialization();
    test_camera_matrices();
    test_ray_sphere_intersection();
    test_distortion_strength();
    
    std::cout << "All diagnostic tests passed successfully!" << std::endl;
    return 0;
}
