#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "blackhole.h"
#include "camera.h"
#include "particlesystem.h"
#include "stars.h"
#include "shader.h"
#include "noises.h"
#include "postprocessor.h"
#include "model.h"

// Constants
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Globals
Camera camera(glm::vec3(0.0f, 1.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float cursorNormalisedX = 0.0f;
float cursorNormalisedY = 0.0f;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    
    cursorNormalisedX = xpos / SCR_WIDTH - 0.5f;
    cursorNormalisedY = ypos / SCR_HEIGHT - 0.5f;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // GLFW init
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "WebGL Black Hole Port", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Not disabled to match WebGL mouse move

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Generate Noise Texture
    GLuint noiseTex = Noises::create(128, 128);

    // Load Shaders
    Shader diskShader("shaders/disk.vert", "shaders/disk.frag");
    Shader horizonShader("shaders/horizon.vert", "shaders/horizon.frag");
    Shader particleShader("shaders/particle.vert", "shaders/particle.frag");
    Shader starsShader("shaders/stars.vert", "shaders/stars.frag");
    Shader distortionActiveShader("shaders/distortion.vert", "shaders/distortion_active.frag");
    Shader distortionMaskShader("shaders/distortion.vert", "shaders/distortion_mask.frag");
    Shader finalShader("shaders/final.vert", "shaders/final.frag");
    Shader cupolaShader("shaders/cupola.vert", "shaders/cupola.frag");

    // Simulation objects
    BlackHole blackHole;
    blackHole.noiseTexture = noiseTex;
    ParticleSystem particleSystem(50000);
    Stars stars(50000);
    Model cupola("resources/models/cupola.glb");
    PostProcessor postProcessor(SCR_WIDTH, SCR_HEIGHT);

    float rotationX = 0.0f;
    float rotationY = 0.0f;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Update camera (matching WebGL Spaceship.js logic but wider range and smoother)
        float targetRotationX = -cursorNormalisedY * 3.5f;
        float targetRotationY = -cursorNormalisedX * 3.5f;
        rotationX += (targetRotationX - rotationX) * deltaTime * 4.0f;
        rotationY += (targetRotationY - rotationY) * deltaTime * 4.0f;

        glm::mat4 camRot = glm::rotate(glm::mat4(1.0f), rotationY, glm::vec3(0, 1, 0));
        camRot = glm::rotate(camRot, rotationX, glm::vec3(1, 0, 0));
        
        glm::vec3 camPos = glm::vec3(0, 0, 2); // Initial offset
        // camPos = glm::vec3(camRot * glm::vec4(camPos, 1.0f)); // If we want to move camera around origin
        
        glm::mat4 view = glm::inverse(glm::translate(glm::mat4(1.0f), camPos) * camRot);
        glm::mat4 projection = glm::perspective(glm::radians(75.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        blackHole.update(deltaTime);

        // Pass 1: Space
        postProcessor.startSpacePass();
        glEnable(GL_DEPTH_TEST);
        stars.draw(starsShader, view, projection, (float)SCR_HEIGHT * 2.0f);
        blackHole.drawSpace(diskShader, horizonShader, view, projection);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        particleSystem.draw(particleShader, view, projection, currentFrame, (float)SCR_HEIGHT * 2.0f);
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Pass 2: Distortion
        postProcessor.startDistortionPass();
        glDisable(GL_BLEND);
        blackHole.drawDistortion(distortionActiveShader, distortionMaskShader, view, projection, camPos);

        // Pass 3: Final
        glm::vec4 screenPos = projection * view * glm::vec4(blackHole.position, 1.0f);
        glm::vec2 bhScreen = glm::vec2(screenPos.x / screenPos.w, screenPos.y / screenPos.w) * 0.5f + 0.5f;
        postProcessor.renderFinal(finalShader, bhScreen, 0.00001f);

        // Pass 4: Overlay (Cupola)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glm::mat4 cupolaModel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, sin(currentFrame * 0.5f) * 6.0f));
        cupolaModel = glm::rotate(cupolaModel, 0.5f, glm::vec3(0, 1, 0));
        cupolaModel = glm::scale(cupolaModel, glm::vec3(0.1f));
        cupola.draw(cupolaShader, cupolaModel, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
