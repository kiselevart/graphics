#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

// Fragment shader for box faces with texture support
const char* texturedFragmentShaderSource = R"(
#version 330 core
out vec4 color;

in vec2 TexCoords; // Texture coordinates from vertex shader
uniform sampler2D textureSampler;

void main()
{
    color = texture(textureSampler, TexCoords);
    // color = vec4(TexCoords, 0.0f, 1.0f);
}
)";

// Vertex shader source code
const char* texturedVertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexTexCoord; // Texture coordinates

out vec2 TexCoords; // Pass texture coordinates to fragment shader

uniform mat4 MVP;

void main()
{
    TexCoords = vertexTexCoord;
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}
)";

// Function to load texture
GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load image data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA; // Determine format based on channels
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data); // Free image memory after loading into OpenGL
    return textureID;
}

// Create box vertices with texture coordinates for all faces
vector<GLfloat> createBoxWithTexCoords(const vec3& c, float w, float h, float d) {
    float mw = w / 2.0f;
    float mh = h / 2.0f;
    float md = d / 2.0f;

    vector<GLfloat> vertices = {
        // Front face         // Position            // Texture coords
        c.x - mw, c.y + mh, c.z - md,    0.0f, 0.0f,  // top-left
        c.x + mw, c.y + mh, c.z - md,    1.0f, 0.0f,  // top-right
        c.x - mw, c.y - mh, c.z - md,    0.0f, 1.0f,  // bottom-left
        c.x + mw, c.y - mh, c.z - md,    1.0f, 1.0f,  // bottom-right

        // Back face
        c.x - mw, c.y + mh, c.z + md,    0.0f, 0.0f,
        c.x + mw, c.y + mh, c.z + md,    1.0f, 0.0f,
        c.x - mw, c.y - mh, c.z + md,    0.0f, 1.0f,
        c.x + mw, c.y - mh, c.z + md,    1.0f, 1.0f,

        // Top face
        c.x - mw, c.y + mh, c.z + md,    0.0f, 0.0f,
        c.x + mw, c.y + mh, c.z + md,    1.0f, 0.0f,
        c.x - mw, c.y + mh, c.z - md,    0.0f, 1.0f,
        c.x + mw, c.y + mh, c.z - md,    1.0f, 1.0f,

        // Bottom face
        c.x - mw, c.y - mh, c.z + md,    0.0f, 0.0f,
        c.x + mw, c.y - mh, c.z + md,    1.0f, 0.0f,
        c.x - mw, c.y - mh, c.z - md,    0.0f, 1.0f,
        c.x + mw, c.y - mh, c.z - md,    1.0f, 1.0f,

        // Right face
        c.x + mw, c.y + mh, c.z - md,    0.0f, 0.0f,
        c.x + mw, c.y + mh, c.z + md,    1.0f, 0.0f,
        c.x + mw, c.y - mh, c.z - md,    0.0f, 1.0f,
        c.x + mw, c.y - mh, c.z + md,    1.0f, 1.0f,

        // Left face
        c.x - mw, c.y + mh, c.z - md,    0.0f, 0.0f,
        c.x - mw, c.y + mh, c.z + md,    1.0f, 0.0f,
        c.x - mw, c.y - mh, c.z - md,    0.0f, 1.0f,
        c.x - mw, c.y - mh, c.z + md,    1.0f, 1.0f
    };

    return vertices;
}

// Update indices to match the new vertex layout
vector<GLuint> createBoxIndices() {
    return {
        // Front face
        0, 1, 2, 2, 3, 1,
        // Back face
        4, 5, 6, 6, 7, 5,
        // Left face
        8, 9, 10, 10, 11, 9,
        // Right face
        12, 13, 14, 14, 15, 13,
        // Top face
        16, 17, 18, 18, 19, 17,
        // Bottom face
        20, 21, 22, 22, 23, 21
    };
}

// Create shader program with error checking
GLuint createShaderProgram(const char* vertexSource, const char* fragSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Vertex Shader Compilation Failed\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR: Fragment Shader Compilation Failed\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Create Textured Box", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD after creating the OpenGL context
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return -1;
    }

    GLuint textureID = loadTexture("wall.jpg");

    // Create vertices, indices for box 
    vector<GLfloat> boxVertices = createBoxWithTexCoords(vec3(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, 1.0f);
    vector<GLuint> boxIndices = createBoxIndices();

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind VAO and buffers for the box
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(GLfloat), boxVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boxIndices.size() * sizeof(GLuint), boxIndices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // Set up shaders
    GLuint texturedShaderProgram = createShaderProgram(texturedVertexShaderSource, texturedFragmentShaderSource);

    glUseProgram(texturedShaderProgram);
    GLint textureLoc = glGetUniformLocation(texturedShaderProgram, "textureSampler");
    glUniform1i(textureLoc, 0);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL Error: " << err << std::endl;
    }

    // Set up camera and projection matrices
    mat4 Projection = perspective(radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    
    // Camera matrix - positioned to see the cube clearly
    mat4 View = lookAt(
        vec3(3,2,-3), // Camera position
        vec3(0,0,0),  // Look at the origin
        vec3(0,1,0)   // Head is up
    );
    
    // Model matrix - static position
    mat4 Model = mat4(1.0f);
    
    // Calculate MVP
    mat4 MVP = Projection * View * Model;

    // Get MVP uniform location for both shader programs
    GLint texturedMVPLoc = glGetUniformLocation(texturedShaderProgram, "MVP");

    // Enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // Add at the beginning of main(), after GLFW initialization:
    float lastTime = glfwGetTime();

    // Replace the render loop with this updated version:
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // Create rotation matrix - rotate around Y axis
        float rotationSpeed = 0.5f; // Radians per second
        mat4 Model = rotate(mat4(1.0f), 
                        currentTime * rotationSpeed, // Angle of rotation
                        vec3(0.0f, 1.0f, 0.0f));    // Rotate around Y axis

        // Calculate new MVP with rotation
        mat4 MVP = Projection * View * Model;

        // Draw the textured box faces
        glUseProgram(texturedShaderProgram);
        glUniformMatrix4fv(texturedMVPLoc, 1, GL_FALSE, value_ptr(MVP));
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, boxIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
