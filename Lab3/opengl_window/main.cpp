#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Set GLFW window properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For Mac compatibility

    // Create a window
    GLFWwindow *mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Window", NULL, NULL);
    if (!mainWindow) {
        std::cout << "GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Get buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    // Set the context for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    // Allow modern extension features
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW initialization failed!" << std::endl;
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return -1;
    }

    // Set viewport size
    glViewport(0, 0, bufferWidth, bufferHeight);

    // Main loop
    while (!glfwWindowShouldClose(mainWindow)) {
        // Clear window
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers
        glfwSwapBuffers(mainWindow);

        // Check for events
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}
