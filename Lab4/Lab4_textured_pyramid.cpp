#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace glm;

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;  // Changed from 'color' to standard 'FragColor'

uniform sampler2D textures[5];
uniform int faceIndex;  // Added uniform for face selection

void main()
{
    FragColor = texture(textures[faceIndex], TexCoords);
}
)";

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexTexCoords;

out vec2 TexCoords;

uniform mat4 MVP;  // Removed duplicate 'model' uniform

void main()
{
    TexCoords = vertexTexCoords;
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}
)";

GLuint loadTexture(const char* filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }

    stbi_image_free(data);
    return textureID;
}


vector<GLfloat> createPyramid(const vec3& c, float baseSize, float height) {
    float half = baseSize / 2.0f;

    vector<GLfloat> vertices = {
        c.x - half, c.y - half, c.z - half,  
        c.x - half, c.y - half, c.z + half,  
        c.x + half, c.y - half, c.z + half,  
        c.x + half, c.y - half, c.z - half,  

        c.x - half, c.y - half, c.z - half,  
        c.x + half, c.y - half, c.z - half,  
        c.x, c.y + height, c.z,

        c.x - half, c.y - half, c.z + half,  
        c.x + half, c.y - half, c.z + half,  
        c.x, c.y + height, c.z,

        c.x - half, c.y - half, c.z - half,  
        c.x - half, c.y - half, c.z + half,  
        c.x, c.y + height, c.z,

        c.x + half, c.y - half, c.z + half,  
        c.x + half, c.y - half, c.z - half,  
        c.x, c.y + height, c.z,
    };

    return vertices;
}

vector<GLuint> createPyramidIndices() {
    return {
        0, 1, 3, 2, 3, 1, 
        4, 5, 6, 
        7, 8, 9, 
        10, 11, 12,  
        13, 14, 15, 
    };
}

vector<GLfloat> createTexCoords() {
    return {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,
    };
}

GLuint compileShader(GLuint shader, const char* shaderSource) {
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* fragSource) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    vertexShader = compileShader(vertexShader, vertexShaderSource);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    fragmentShader = compileShader(fragmentShader, fragSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


int main() {
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "Draw Textured Pyramid", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        return -1;
    }

    vec3 pyramidCenter(0.0f, 0.0f, 0.0f);

    // creating list of textures for each face
    GLuint texture1 = loadTexture("texture1.jpg");
    GLuint texture2 = loadTexture("texture2.jpg");
    GLuint texture3 = loadTexture("texture3.jpg");
    GLuint texture4 = loadTexture("texture4.jpg");
    GLuint textureBase = loadTexture("textureBase.jpg");
    GLuint textures[] = {texture1, texture2, texture3, texture4, textureBase};
    int numIdenticalFaces = 4;

    vector<GLfloat> pyramidVertices = createPyramid(pyramidCenter, 1.0f, 1.5f);
    vector<GLuint> pyramidIndices = createPyramidIndices();
    vector<GLfloat> pyramidTexCoords = createTexCoords();

    GLuint VBO, VAO, EBO, TBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &TBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pyramidVertices.size() * sizeof(GLfloat), pyramidVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramidIndices.size() * sizeof(GLuint), pyramidIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, pyramidTexCoords.size() * sizeof(GLfloat), pyramidTexCoords.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    GLuint shaderProgram = createShaderProgram(fragmentShaderSource);

    float width = 1024.0f;  
    float height = 768.0f;  
    float aspectRatio = width / height;

    mat4 Projection = perspective(radians(45.0f),  
                                aspectRatio,        
                                0.1f,             
                                100.0f);           

    mat4 View = lookAt(
        vec3(3.0f, 3.0f, 2.0f),   
        vec3(0.0f, 0.0f, 0.0f),    
        vec3(0.0f, 1.0f, 0.0f)   
    );

    mat4 Model = mat4(1.0f);  // Identity matrix
    Model = translate(Model, vec3(0.0f, -0.5f, 0.0f));   
    mat4 MVP = Projection * View * Model;

    GLuint MVPLocation = glGetUniformLocation(shaderProgram, "MVP");
    glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, value_ptr(MVP));

    glViewport(0, 0, 1024, 768);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Update MVP matrix if you want rotation
        Model = rotate(Model, radians(0.5f), vec3(0.0f, 1.0f, 0.0f));
        MVP = Projection * View * Model;
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, value_ptr(MVP));

        // Bind all textures at once
        for (int i = 0; i < 5; ++i) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glUniform1i(glGetUniformLocation(shaderProgram, ("textures[" + to_string(i) + "]").c_str()), i);
        }

        // Draw the pyramid faces
        GLint faceIndexLoc = glGetUniformLocation(shaderProgram, "faceIndex");
        
        // Draw the four side faces
        for (int i = 0; i < 4; ++i) {
            glUniform1i(faceIndexLoc, i);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(GLuint)));
        }

        // Draw the base face
        glUniform1i(faceIndexLoc, 4);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(GLuint)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}
