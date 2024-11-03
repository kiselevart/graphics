#include <vector>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

// Vertex shader source code
const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}
)";

// Fragment shader for pyramid faces
const char* fragmentShaderSource = R"(
#version 330 core

out vec3 color;
void main()
{
    color = vec3(1, 0, 0);
}
)";

// Fragment shader for pyramid edges
const char* edgeFragmentShaderSource = R"(
#version 330 core

out vec3 color;
void main()
{
    color = vec3(0, 0, 0);
}
)";

GLFWwindow* window;

vector<GLfloat> createPyramid(const vec3& c, float baseSize, float height) {
    float halfBase = baseSize / 2.0f;

    vector<GLfloat> vertices = {
        // Bottom vertices
        c.x - halfBase, c.y, c.z - halfBase, // 0
        c.x + halfBase, c.y, c.z - halfBase, // 1
        c.x + halfBase, c.y, c.z + halfBase, // 2
        c.x - halfBase, c.y, c.z + halfBase, // 3

        // Top vertex
        c.x, c.y + height, c.z              // 4
    };

    return vertices;
}

vector<GLuint> createPyramidIndices() {
    const vector<GLuint> indices = {
        // Base (square)
        0, 1, 2, 0, 2, 3,

        // Sides (triangles)
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    return indices;
}

vector<GLuint> createEdgeIndices() {
    const vector<GLuint> edgeIndices = {
        // Base edges
        0, 1, 1, 2, 2, 3, 3, 0,

        // Side edges
        0, 4, 1, 4, 2, 4, 3, 4
    };

    return edgeIndices;
}

GLuint createShaderProgram(const char* fragSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

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

    window = glfwCreateWindow(1024, 768, "Draw Square Pyramid With Edges", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    vec3 pyramidCenter(0.0f, 0.0f, 0.0f);
    float baseSize = 1.0f;
    float height = 1.5f;

    vector<GLfloat> pyramidVertices = createPyramid(pyramidCenter, baseSize, height);
    vector<GLuint> pyramidIndices = createPyramidIndices();
    vector<GLuint> edgeIndices = createEdgeIndices();

    GLuint VBO, VAO, EBO, edgeEBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &edgeEBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pyramidVertices.size() * sizeof(GLfloat), pyramidVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices.size() * sizeof(GLuint), pyramidIndices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, edgeIndices.size() * sizeof(GLuint), edgeIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    GLuint faceShaderProgram = createShaderProgram(fragmentShaderSource);
    GLuint edgeShaderProgram = createShaderProgram(edgeFragmentShaderSource);

    glEnable(GL_DEPTH_TEST);

    mat4 view = lookAt(vec3(3.0f, 3.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 model = mat4(1.0f);
        mat4 MVP = projection * view * model;

        // Draw pyramid faces
        glUseProgram(faceShaderProgram);
        GLuint mvpLoc = glGetUniformLocation(faceShaderProgram, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(MVP));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, pyramidIndices.size(), GL_UNSIGNED_INT, 0);

        // Draw pyramid edges
        glUseProgram(edgeShaderProgram);
        mvpLoc = glGetUniformLocation(edgeShaderProgram, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(MVP));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, edgeEBO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_LINES, edgeIndices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &edgeEBO);
    glDeleteProgram(faceShaderProgram);
    glDeleteProgram(edgeShaderProgram);
    glfwTerminate();
    return 0;
}
