#include <GL/glew.h>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core

out vec3 color;
void main()
{
    color = vec3(1, 0, 0);
}
)";

const char* edgeFragmentShaderSource = R"(
#version 330 core

out vec3 color;
void main()
{
    color = vec3(0, 0, 0);
}
)";

GLFWwindow* window;

vector<GLfloat> createSphere(float radius, int sectorCount, int stackCount) {
    vector<GLfloat> vertices;
    float x, y, z, xy;
    float stackAngle;

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    return vertices;
}

vector<GLuint> createSphereIndices(int sectorCount, int stackCount) {
    vector<GLuint> indices;

    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            int curRow = i * (sectorCount + 1);
            int nextRow = (i + 1) * (sectorCount + 1);

            indices.push_back(curRow + j);
            indices.push_back(nextRow + j);
            indices.push_back(nextRow + j + 1);

            indices.push_back(curRow + j);
            indices.push_back(nextRow + j + 1);
            indices.push_back(curRow + j + 1);
        }
    }
    return indices;
}

vector<GLuint> createEdgeIndices(int sectorCount, int stackCount) {
    vector<GLuint> edgeIndices;

    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            edgeIndices.push_back(i * (sectorCount + 1) + j);
            edgeIndices.push_back(i * (sectorCount + 1) + j + 1);
        }
    }

    for (int j = 0; j <= sectorCount; ++j) {
        for (int i = 0; i < stackCount; ++i) {
            edgeIndices.push_back(i * (sectorCount + 1) + j);
            edgeIndices.push_back((i + 1) * (sectorCount + 1) + j);
        }
    }

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

    window = glfwCreateWindow(1024, 768, "Draw Sphere With Edges", NULL, NULL);
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

    float radius = 1.0f;
    int sectorCount = 36;
    int stackCount = 18;

    vector<GLfloat> sphereVertices = createSphere(radius, sectorCount, stackCount);
    vector<GLuint> sphereIndices = createSphereIndices(sectorCount, stackCount);
    vector<GLuint> edgeIndices = createEdgeIndices(sectorCount, stackCount);

    GLuint VBO, VAO, EBO, edgeEBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &edgeEBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(GLfloat), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(GLuint), sphereIndices.data(), GL_STATIC_DRAW);

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

        glUseProgram(faceShaderProgram);
        GLuint mvpLoc = glGetUniformLocation(faceShaderProgram, "MVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(MVP));

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);

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
