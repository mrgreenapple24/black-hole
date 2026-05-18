cmake_minimum_required(VERSION 3.16)
project(BlackholeSimulation VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# GLFW
find_package(glfw3 QUIET)
if(NOT glfw3_FOUND)
  if(EXISTS "${CMAKE_SOURCE_DIR}/external/glfw/CMakeLists.txt")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
  else()
    message(FATAL_ERROR "GLFW not found. Install with: sudo apt install libglfw3-dev\nor place GLFW source in external/glfw/")
  endif()
endif()

# GLAD
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# GLM (header-only)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE external/glm)

# OpenGL
find_package(OpenGL REQUIRED)

# Sources
set(SOURCES
    src/main.cpp
    src/shader.cpp
    src/camera.cpp
    src/blackhole.cpp
    src/particlesystem.cpp
)

add_executable(blackhole ${SOURCES})

target_include_directories(blackhole PRIVATE include src external/glad/include external/glm)
target_link_libraries(blackhole PRIVATE OpenGL::GL glfw glad glm)

if(UNIX AND NOT APPLE)
  target_link_libraries(blackhole PRIVATE dl pthread)
endif()
if(APPLE)
  target_link_libraries(blackhole PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreFoundation")
endif()

# Copy assets
add_custom_command(TARGET blackhole POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
)
