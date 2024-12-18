#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Create an OpenGL windowed context
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Version Check", NULL, NULL);
    if (!window) {
        std::cerr << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Retrieve OpenGL version
    const char* version = (const char*)glGetString(GL_VERSION);
    std::cout << "OpenGL Version: " << version << std::endl;

    // Wait before closing the window
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
