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
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
}

)";

// Fragment shader source code
const char* fragmentShaderSource = R"(
#version 330 core

out vec3 color;
void main()
{
	color = vec3(1,0,0);
}
)";

GLFWwindow* window;

vector<GLfloat> createBox(const vec3& center, float width, float height, float depth) {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    float halfDepth = depth / 2.0f;

    vector<GLfloat> vertices = {
        center.x - halfWidth, center.y - halfHeight, center.z + halfDepth,
        center.x + halfWidth, center.y - halfHeight, center.z + halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z + halfDepth,
        center.x - halfWidth, center.y + halfHeight, center.z + halfDepth,
        
        center.x - halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z - halfDepth,
        center.x - halfWidth, center.y + halfHeight, center.z - halfDepth,

        center.x - halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x - halfWidth, center.y - halfHeight, center.z + halfDepth,
        center.x - halfWidth, center.y + halfHeight, center.z + halfDepth,
        center.x - halfWidth, center.y + halfHeight, center.z - halfDepth,

        center.x + halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y - halfHeight, center.z + halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z + halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z - halfDepth,

        center.x - halfWidth, center.y + halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y + halfHeight, center.z + halfDepth,
        center.x - halfWidth, center.y + halfHeight, center.z + halfDepth,

        center.x - halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y - halfHeight, center.z - halfDepth,
        center.x + halfWidth, center.y - halfHeight, center.z + halfDepth,
        center.x - halfWidth, center.y - halfHeight, center.z + halfDepth,
    };

    return vertices;
}

vector<GLuint> createBoxIndices() {
    return {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };
}

GLuint createShaderProgram() {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
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

    window = glfwCreateWindow(800, 600, "Create Box", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; 
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    vec3 boxCenter(0.0f, 0.0f, 0.0f);
    float boxWidth = 1.0f;
    float boxHeight = 1.0f;
    float boxDepth = 1.0f;

    vector<GLfloat> boxVertices = createBox(boxCenter, boxWidth, boxHeight, boxDepth);
    vector<GLuint> boxIndices = createBoxIndices();

    GLuint VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(GLfloat), boxVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boxIndices.size() * sizeof(GLuint), boxIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    GLuint shaderProgram = createShaderProgram();

    mat4 view = lookAt(vec3(3.0f, 3.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    mat4 projection = perspective(radians(45.0f), 1024.0f / 768.0f, 0.1f, 100.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST); 

        glUseProgram(shaderProgram);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        mat4 model = mat4(1.0f); 
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, boxIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
