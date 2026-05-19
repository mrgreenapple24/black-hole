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

PostProcessor* g_postProcessor = nullptr;
unsigned int g_scrWidth = SCR_WIDTH;
unsigned int g_scrHeight = SCR_HEIGHT;

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    g_scrWidth = width;
    g_scrHeight = height;
    if (g_postProcessor) {
        g_postProcessor->resize(width, height);
    }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.processMouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processScroll(static_cast<float>(yoffset));
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera.processKeyboard(0, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera.processKeyboard(1, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera.processKeyboard(2, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera.processKeyboard(3, deltaTime);
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
    g_postProcessor = &postProcessor;

    float rotationX = 0.0f;
    float rotationY = 0.0f;

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 view = camera.viewMatrix();
        glm::mat4 projection = camera.projMatrix((float)g_scrWidth / (float)g_scrHeight);

        blackHole.update(deltaTime);

        // Pass 1: Space
        postProcessor.startSpacePass();
        glEnable(GL_DEPTH_TEST);
        stars.draw(starsShader, view, projection, (float)g_scrHeight * 2.0f);
        blackHole.drawSpace(diskShader, horizonShader, view, projection);
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        particleSystem.draw(particleShader, view, projection, currentFrame, (float)g_scrHeight * 2.0f);
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Pass 2: Distortion
        postProcessor.startDistortionPass();
        glDisable(GL_DEPTH_TEST);
        blackHole.drawDistortion(distortionActiveShader, distortionMaskShader, view, projection, camera.position);
        glEnable(GL_DEPTH_TEST);

        // Pass 3: Final
        glm::vec4 screenPos = projection * view * glm::vec4(blackHole.position, 1.0f);
        glm::vec2 bhScreen;
        if (screenPos.w > 0.0f) {
            bhScreen = glm::vec2(screenPos.x / screenPos.w, screenPos.y / screenPos.w) * 0.5f + 0.5f;
        } else {
            bhScreen = glm::vec2(-10.0f, -10.0f);
        }
        postProcessor.renderFinal(finalShader, bhScreen, 0.00001f);

        // Pass 4: Overlay (Cupola)
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        // Attach cupola to camera
        glm::mat4 cupolaModel = glm::translate(glm::mat4(1.0f), camera.position + camera.front * 0.2f - camera.up * 0.08f);
        cupolaModel = glm::rotate(cupolaModel, glm::radians(-camera.yaw - 90.0f), glm::vec3(0, 1, 0));
        cupolaModel = glm::rotate(cupolaModel, glm::radians(camera.pitch), glm::vec3(1, 0, 0));
        cupolaModel = glm::scale(cupolaModel, glm::vec3(0.01f));
        cupola.draw(cupolaShader, cupolaModel, view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
