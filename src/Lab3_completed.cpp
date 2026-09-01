#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include "BezierCurve.h"
#include "Particle.h"

using namespace glm;

#include <cmath>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION  //
#include "stb_image.h"              // This includes the header for image loading


//function for particles with transparency
unsigned int loadParticleTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    int width, height, nrChannels;
    // RGBA  for transparency
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
    
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        std::cout << "✅ Loaded particle texture: " << path << " (" << width << "x" << height << ")" << std::endl;
        return textureID;
    } else {
        std::cout << "❌ Failed to load particle texture: " << path << std::endl;
        std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
        return 0;
    }
}
// loadCubemap function
unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        std::cout << "Loaded texture: " << path << std::endl;
    } else {
        std::cout << "Failed to load texture: " << path << std::endl;
    }
    
    return textureID;
}
// Function to generate sphere vertices
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            // Determine format based on number of channels
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
            
            std::cout << "Loaded: " << faces[i] << " (" << width << "x" << height << ")" << std::endl;
        } else {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            std::cout << "STB Error: " << stbi_failure_reason() << std::endl;
        }
    }
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}



std::vector<float> createParticleQuad() {
    std::vector<float> vertices;
    
    // Vertex format: position (xyz), texCoord (uv)
    // Two triangles forming a quad
    vertices.insert(vertices.end(), {
        // Triangle 1
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        
        // Triangle 2
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f
    });
    
    return vertices;
}

// function near the top of your main.cpp
std::vector<float> createTexturedCube() {
    std::vector<float> vertices;
    
    // Front face (z = 1.0) - 2 triangles (6 vertices)
    vertices.insert(vertices.end(), {
        // position (x,y,z,w)     normal (x,y,z,w)        color (r,g,b,a)        texCoord (u,v)
        // Triangle 1
        -1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        
        // Triangle 2
         1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
        
        // Back face (z = -1.0)
        -1.0f, -1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
         1.0f,  1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        
         1.0f,  1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
         1.0f, -1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,   0.0f, 0.0f, -1.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
        
        // Left face (x = -1.0)
        -1.0f, -1.0f, -1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        
        -1.0f,  1.0f,  1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,   -1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
        
        // Right face (x = 1.0)
         1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        
         1.0f,  1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
        
        // Top face (y = 1.0)
        -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
        
         1.0f,  1.0f,  1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
         1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f, -1.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        
        // Bottom face (y = -1.0)
        -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f,
         1.0f, -1.0f, -1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        
         1.0f, -1.0f,  1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f, 1.0f,    0.0f, -1.0f, 0.0f, 0.0f,   1.0f,1.0f,1.0f,1.0f,   0.0f, 0.0f
    });
    
    return vertices;
}

std::vector<float> generateSolidSphere(float radius, int sectors, int stacks) {
    std::vector<float> vertices;
    
    for (int i = 0; i <= stacks; ++i) {
        float V = (float)i / (float)stacks;
        float phi = V * M_PI;  // polar angle
        
        for (int j = 0; j <= sectors; ++j) {
            float U = (float)j / (float)sectors;
            float theta = U * 2.0f * M_PI;  // azimuthal angle
            
            // Calculate vertex position
            float x = radius * sin(phi) * cos(theta);
            float y = radius * sin(phi) * sin(theta);
            float z = radius * cos(phi);
            
            // Normal 
            float nx = x / radius;
            float ny = y / radius;
            float nz = z / radius;
            
            // Add vertex: position (xyz1) + normal (xyzw) + color (rgba)
            vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); vertices.push_back(1.0f);
            vertices.push_back(nx); vertices.push_back(ny); vertices.push_back(nz); vertices.push_back(0.0f);
            vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
        }
    }
    
    // Generate indices for triangles
    std::vector<unsigned int> indices;
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < sectors; ++j) {
            int first = i * (sectors + 1) + j;
            int second = first + sectors + 1;
            
            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);
            
            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
    
    // Convert to flat array for OpenGL
    std::vector<float> finalVertices;
    for (unsigned int idx : indices) {
        int base = idx * 12; // 12 floats per vertex
        for (int k = 0; k < 12; ++k) {
            finalVertices.push_back(vertices[base + k]);
        }
    }
    
    return finalVertices;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    //GLFW init
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Street Scene", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD\n"; return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Camera camera(window, 90.0f, 3.0f);
 //   BezierCurve cameraPath;
    // Portal camera (attached to car)
    Camera portalCamera(window, 90.0f, 3.0f);



//     std::vector<glm::vec3> controlPoints = {
//     glm::vec3(10.0f, 6.0f, 0.0f),   // Right side
//     glm::vec3(5.0f, 5.0f, -8.0f),   // Front right
//     glm::vec3(0.0f, 5.0f, -10.0f),  // Front center
//     glm::vec3(-5.0f, 5.0f, -8.0f),  // Front left
//     glm::vec3(-10.0f, 5.0f, 0.0f),  // Left side
//     glm::vec3(-5.0f, 5.0f, 8.0f),   // Back left
//     glm::vec3(0.0f, 5.0f, 10.0f),   // Back center
//     glm::vec3(5.0f, 5.0f, 8.0f),    // Back right
//     glm::vec3(10.0f, 6.0f, 0.0f)    // Back to start
// };

    std::vector<glm::vec3> controlPoints = {
        glm::vec3(10,6,0),
        glm::vec3(5,5,-8),
        glm::vec3(-5,5,-8),
        glm::vec3(-10,5,0)
    };

    
  //  cameraPath.setControlPoints(controlPoints);
  //  camera.setPath(&cameraPath);
    // Create a bezier curve for the sun
    BezierCurve sunPath;

    std::vector<glm::vec3> sunControlPoints = {
        // Start - right side, high
        glm::vec3(15.0f, 12.0f, 5.0f),
        
        // Move across the sky
        glm::vec3(5.0f, 14.0f, 2.0f),
        glm::vec3(0.0f, 15.0f, 0.0f),   // high in the sky
        
        // go down
        glm::vec3(-5.0f, 12.0f, -2.0f),
        glm::vec3(-10.0f, 8.0f, -5.0f),  // Sunset
        
        // Reset back to start
        glm::vec3(15.0f, 12.0f, 5.0f)
    };

    sunPath.setControlPoints(sunControlPoints);
    
    // Variables for camera animation
    float animationTime = 0.0f;        // Current progress 
    float animationSpeed = 0.1f;       // Speed of movement 
    bool animateCamera = true; 

    Shader shader("src/basic.shader");
    Shader lightShader("src/light.shader");
    Shader skyboxShader("src/skybox.shader");
    Shader normalShader("src/normal_mapping.shader");
    // After createing  shaders 
    Shader toonShader("src/toon.shader");
    Shader outlineShader("src/outline.shader");
    shader.Bind();
    lightShader.Bind();
    skyboxShader.Bind();
    normalShader.Bind();

    // After creating  shaders
    Shader particleShader("src/particle.shader");


        // Portal/Framebuffer setup
    unsigned int portalFBO, portalTexture, portalRBO;
    int portalWidth = 512, portalHeight = 512;  // Smaller for performance


        // Sun quad vertices (billboard)
    float sunVertices[] = {
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f
    };

    unsigned int sunVAO, sunVBO;
    glGenVertexArrays(1, &sunVAO);
    glGenBuffers(1, &sunVBO);
    glBindVertexArray(sunVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sunVertices), sunVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load sun shader
  //  Shader sunShader("src/sun_billboard.shader");
    // Portal surface vertices (a rectangle)
    float portalVertices[] = {
        // positions          // texture coordinates
        -0.8f, -0.6f, 0.0f,   0.0f, 0.0f,
        0.8f, -0.6f, 0.0f,   1.0f, 0.0f,
        0.8f,  0.6f, 0.0f,   1.0f, 1.0f,
        0.8f,  0.6f, 0.0f,   1.0f, 1.0f,
        -0.8f,  0.6f, 0.0f,   0.0f, 1.0f,
        -0.8f, -0.6f, 0.0f,   0.0f, 0.0f
    };

    unsigned int portalVAO, portalVBO;
    glGenVertexArrays(1, &portalVAO);
    glGenBuffers(1, &portalVBO);
    glBindVertexArray(portalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, portalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(portalVertices), portalVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create framebuffer
    glGenFramebuffers(1, &portalFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, portalFBO);

    // Create texture to render to
    glGenTextures(1, &portalTexture);
    glBindTexture(GL_TEXTURE_2D, portalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, portalWidth, portalHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, portalTexture, 0);

    // Create renderbuffer for depth
    glGenRenderbuffers(1, &portalRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, portalRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, portalWidth, portalHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, portalRBO);

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR: Portal framebuffer not complete!" << std::endl;
    } else {
        std::cout << "Portal framebuffer ready!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Load portal shader
    Shader portalShader("src/portal.shader");

    // Create particle quad
    std::vector<float> particleQuad = createParticleQuad();
    unsigned int particleVAO, particleVBO;
    glGenVertexArrays(1, &particleVAO);
    glGenBuffers(1, &particleVBO);
    glBindVertexArray(particleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    glBufferData(GL_ARRAY_BUFFER, particleQuad.size() * sizeof(float), particleQuad.data(), GL_STATIC_DRAW);

    // Position attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Load your snowflake PNG texture
    unsigned int snowflakeTexture = loadParticleTexture("include/assets/snowflake.png");  // Update path to your PNG file

    // Check if texture loaded
    if (snowflakeTexture == 0) {
        std::cout << "Not found bruhe!" << std::endl;
    }

    // Create particle system
    ParticleSystem snowSystem;
    snowSystem.setGravity(glm::vec3(0.0f, -0.8f, 0.0f));
    // In  render loop, update spawn area to follow camera
   // glm::vec3 cameraPos = camera.getPosition();
    // In main(), set spawn area to cover the entire sky volume
    // snowSystem.setSpawnArea(
    //     glm::vec3(-25.0f, 15.0f, -25.0f),   // Min Y = 15 (above all buildings)
    //     glm::vec3(25.0f, 40.0f, 25.0f)      // Max Y = 40 (high in the sky)
    // );
    // snowSystem.setSpawnArea(glm::vec3(-20.0f, 15.0f, -20.0f), glm::vec3(20.0f, 30.0f, 20.0f));
    // snowSystem.setGravity(glm::vec3(0.0f, -1.2f, 0.0f));

        // Create textured cube model
    Model texturedBuilding;
    std::vector<float> texturedCubeData = createTexturedCube();
    texturedBuilding.initWithUV(texturedCubeData);

    // Load textures (update paths to your downloaded textures)
    
    unsigned int diffuseTexture = loadTexture("include/assets/textures/plaster_brick_pattern_diff_4k.jpg");
    unsigned int normalTexture = loadTexture("include/assets/textures/plaster_brick_pattern_nor_gl_4k.jpg");

        // In main() 
    std::vector<std::string> faces = {
        "include/assets/skybox/px.png",   // +X
        "include/assets/skybox/nx.png",    // -X
        "include/assets/skybox/py.png",     // +Y
        "include/assets/skybox/ny.png",  // -Y
        "include/assets/skybox/pz.png",   // +Z
        "include/assets/skybox/nz.png"     // -Z
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    //Creation of Models
    Model roadCube, buildingCube, carModel, sidewalkCube;
    Model lightSphere;
    std::vector<float> sphereVertices = generateSolidSphere(1.0f, 36, 18); // radius 1, 36 sectors, 18 stacks
    lightSphere.init(sphereVertices.data(), sphereVertices.size());


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //Simple cube for road/sidewalk/buildings
    float cubeVertices[] = {
        // pos(xyz1) normal(xyzw) color(rgba)
		-1.0f,-1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
												
		1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
												
		1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
												
		1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
												
		-1.0f,-1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
												
		1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,

		-1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f, 1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
											   
		1.0f,-1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f, 1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f
    };

    float buildingVertices[] = {
        // pos(xyz1) normal(xyzw) color(rgba)
		-1.0f,-1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	1.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	1.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	1.0f,1.0f,1.0f,1.0f,
												
		1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
												
		1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
												
		1.0f, 1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,0.0f,-1.0f,0.0f,	1.0f,1.0f,0.0f,1.0f,
												
		-1.0f,-1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	-1.0f,0.0f,0.0f,0.0f,	0.0f,1.0f,1.0f,1.0f,
												
		1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,1.0,	0.0f,-1.0f,0.0f,0.0f,	1.0f,0.0f,1.0f,1.0f,

		-1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		-1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f,-1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f, 1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
											   
		1.0f,-1.0f,-1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f, 1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	1.0f,0.0f,0.0f,0.0f,	1.0f,0.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f,-1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	0.0f,1.0f,0.0f,0.0f,	0.0f,1.0f,0.0f,1.0f,
											   
		1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		-1.0f, 1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f,
		1.0f,-1.0f, 1.0f,1.0,	0.0f,0.0f,1.0f,0.0f,	0.0f,0.0f,1.0f,1.0f
    };
    buildingCube.init(buildingVertices, sizeof(buildingVertices)/sizeof(float));
    roadCube.init(cubeVertices, sizeof(cubeVertices)/sizeof(float));
    sidewalkCube.init(cubeVertices, sizeof(cubeVertices)/sizeof(float));
  //  lightCube.init(cubeVertices, sizeof(cubeVertices)/sizeof(float));



    //Loading car OBJ
    std::string carPath = "include/assets/race.obj";
    carModel.init(carPath); 
   // camera.position = glm::vec3(0.0f, 3.0f, 10.0f); 

    glm::vec3 lightPos = glm::vec3(0.0f, 12.0f, 0.0f);  // Starting position
    float lightSpeed = 3.0f;  // Movement speed
    float sunProgress = 0.0f;
    float sunSpeed = 0.05f; 


    //Main loop
    while(!glfwWindowShouldClose(window)) {
        float time = glfwGetTime();
        glfwPollEvents();
        float currentTime = glfwGetTime();
        static float lastTime = currentTime;
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
         // speed as needed

        camera.update(deltaTime);
        // Update sun position along bezier curve
        sunProgress += deltaTime * sunSpeed;
        if (sunProgress >= 1.0f) {
            sunProgress = 0.0f;  // Loop back to start
        }

        // Get new sun position
        lightPos = sunPath.getPosition(sunProgress);
        
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Update light position 
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) lightPos.z -= lightSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) lightPos.z += lightSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) lightPos.x -= lightSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) lightPos.x += lightSpeed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) lightPos.y += lightSpeed * deltaTime;  // Move UP
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) lightPos.y -= lightSpeed * deltaTime; 
        
        // snow spawning location 
        glm::vec3 cameraPos = camera.getPosition();
        glm::vec3 cameraForward = camera.getForward();
        glm::vec3 spawnCenter = cameraPos + cameraForward * 8.0f;
        
        snowSystem.setSpawnArea(
            glm::vec3(spawnCenter.x - 12.0f, cameraPos.y + 2.0f, spawnCenter.z - 12.0f),
            glm::vec3(spawnCenter.x + 12.0f, cameraPos.y + 18.0f, spawnCenter.z + 12.0f)
        );

        

        //drawing all opaqur
        
        // road, buildings, sidewalks, car
        shader.Bind();
        camera.sendToShader(&shader);
        shader.SetUniform3f("u_viewPos", camera.getPosition());
        shader.SetUniform3f("u_lightPos", lightPos);
        
        // Draw road
        glm::mat4 roadModel = glm::mat4(1.0f);
        roadModel = glm::translate(roadModel, glm::vec3(0.0f, -0.5f, 0.0f));
        roadModel = glm::scale(roadModel, glm::vec3(10.0f, 0.1f, 20.0f));
        roadCube.update(roadModel);
        shader.SetUniform4m("u_model", roadModel);
        shader.SetUniform1f("u_useOverrideColor", 1.0f);
        shader.SetUniform3f("u_overrideColor", glm::vec3(0.12f, 0.12f, 0.12f));
        roadCube.draw(&shader);
        shader.SetUniform1f("u_useOverrideColor", 0.0f);
        
       
        // building with toon

        // Set up the model matrix for blue building
        glm::mat4 endBuilding2 = glm::mat4(1.0f);
        endBuilding2 = glm::translate(endBuilding2, glm::vec3(-8.0f, 3.4f, -5.0f));
        endBuilding2 = glm::scale(endBuilding2, glm::vec3(1.0f, 4.0f, 1.0f));
        buildingCube.update(endBuilding2);

        // Draw BLACK OUTLINE 
        glDepthMask(GL_FALSE);  // Disable depth writing so outline doesn't interfere

        outlineShader.Bind();
        outlineShader.SetUniform4m("u_view", camera.getViewMatrix());
        outlineShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        outlineShader.SetUniform4m("u_model", endBuilding2);
        buildingCube.draw(&outlineShader);

        glDepthMask(GL_TRUE);  // Re-enable depth writing

        // : Draw TOON BUILDING (with cel shading)
        toonShader.Bind();
        toonShader.SetUniform3f("u_lightPos", lightPos);
        toonShader.SetUniform3f("u_viewPos", camera.getPosition());
        toonShader.SetUniform4m("u_view", camera.getViewMatrix());
        toonShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        toonShader.SetUniform4m("u_model", endBuilding2);
        toonShader.SetUniform1f("u_useOverrideColor", 1.0f);
        toonShader.SetUniform3f("u_overrideColor", glm::vec3(0.2f, 0.3f, 0.8f));  // Bright blue
        toonShader.SetUniform1f("u_lightIntensity", 0.4f);
        buildingCube.draw(&toonShader);
        


        // First, set up the model matrix
        glm::mat4 endBuilding3 = glm::mat4(1.0f);
        endBuilding3 = glm::translate(endBuilding3, glm::vec3(-8.0f, 2.5f, -8.0f));
        endBuilding3 = glm::scale(endBuilding3, glm::vec3(1.0f, 3.0f, 1.0f));
        buildingCube.update(endBuilding3);

   
        glDepthMask(GL_FALSE);  // Disable depth writing so outline doesn't interfere

        outlineShader.Bind();  // You need to create this shader
        outlineShader.SetUniform4m("u_view", camera.getViewMatrix());
        outlineShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        outlineShader.SetUniform4m("u_model", endBuilding3);
        buildingCube.draw(&outlineShader);

        glDepthMask(GL_TRUE);  // Re-enable depth writing

 
        toonShader.Bind();
        toonShader.SetUniform3f("u_lightPos", lightPos);
        toonShader.SetUniform3f("u_viewPos", camera.getPosition());
        toonShader.SetUniform4m("u_view", camera.getViewMatrix());
        toonShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        toonShader.SetUniform4m("u_model", endBuilding3);
        toonShader.SetUniform1f("u_useOverrideColor", 1.0f);
        toonShader.SetUniform3f("u_overrideColor", glm::vec3(0.2f, 0.7f, 0.7f));  // Cyan color
        buildingCube.draw(&toonShader);
        
        // Draw sidewalks
        glm::mat4 sidewalkLeft = glm::mat4(1.0f);
        sidewalkLeft = glm::translate(sidewalkLeft, glm::vec3(-8.0f, -0.45f, 0.0f));
        sidewalkLeft = glm::scale(sidewalkLeft, glm::vec3(2.0f, 0.1f, 20.0f));
        sidewalkCube.update(sidewalkLeft);
        shader.SetUniform4m("u_model", sidewalkLeft);
        shader.SetUniform1f("u_useOverrideColor", 1.0f);
        shader.SetUniform3f("u_overrideColor", glm::vec3(0.5f, 0.5f, 0.5f));
        sidewalkCube.draw(&shader);
        shader.SetUniform1f("u_useOverrideColor", 0.0f);
        
        glm::mat4 sidewalkRight = glm::mat4(1.0f);
        sidewalkRight = glm::translate(sidewalkRight, glm::vec3(8.0f, -0.45f, 0.0f));
        sidewalkRight = glm::scale(sidewalkRight, glm::vec3(2.0f, 0.1f, 20.0f));
        sidewalkCube.update(sidewalkRight);
        shader.SetUniform4m("u_model", sidewalkRight);
        shader.SetUniform1f("u_useOverrideColor", 1.0f);
        shader.SetUniform3f("u_overrideColor", glm::vec3(0.5f, 0.5f, 0.5f));
        sidewalkCube.draw(&shader);
        shader.SetUniform1f("u_useOverrideColor", 0.0f);
        
        // // Draw car
        // float t = time;
        // glm::vec3 carPos = glm::vec3(5.0f * sin(t), -0.4f, 2.5f * sin(t) * cos(t));
        // glm::mat4 carModelMat = glm::mat4(1.0f);
        // carModelMat = glm::translate(carModelMat, carPos);
        // carModelMat = glm::rotate(carModelMat, -t + glm::radians(90.0f), glm::vec3(0,1,0));
        // carModelMat = glm::scale(carModelMat, glm::vec3(0.5f));
        // carModel.update(carModelMat);

        // Draw car with outline
        float t = time;
        glm::vec3 carPos = glm::vec3(5.0f * sin(t), -0.4f, 2.5f * sin(t) * cos(t));
        glm::mat4 carModelMat = glm::mat4(1.0f);
        carModelMat = glm::translate(carModelMat, carPos);
        carModelMat = glm::rotate(carModelMat, -t + glm::radians(90.0f), glm::vec3(0,1,0));
        carModelMat = glm::scale(carModelMat, glm::vec3(0.5f));
        carModel.update(carModelMat);
        // Get camera position
      //  glm::vec3 cameraPos = camera.getPosition();
        // // After updating car position
        // camera.setCarTarget(carPos);

        // if (cameraPos.y > 7.0f) {
        //     camera.lookAt(carPos);
        // } else {
        //     camera.clearLookAt();
        // }


        //Draw outline (slightly larger, black)
        glDepthMask(GL_FALSE);  // Disable depth writing for outline
        outlineShader.Bind();
        outlineShader.SetUniform4m("u_model", carModelMat);
        outlineShader.SetUniform4m("u_view", camera.getViewMatrix());
        outlineShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        carModel.draw(&outlineShader);
        glDepthMask(GL_TRUE);   // Re-enable depth writing

        // Step 2: Draw toon car on top
        toonShader.Bind();
        toonShader.SetUniform3f("u_lightPos", lightPos);
        toonShader.SetUniform3f("u_viewPos", camera.getPosition());
        toonShader.SetUniform4m("u_view", camera.getViewMatrix());
        toonShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        toonShader.SetUniform4m("u_model", carModelMat);
        toonShader.SetUniform1f("u_useOverrideColor", 1.0f); 
        toonShader.SetUniform3f("u_overrideColor", glm::vec3(1.0f, 0.2f, 0.2f));
        toonShader.SetUniform1f("u_lightIntensity", 0.4f);  // ← Add this! (0.4 = 40% brightness)
    //     toonShader.SetUniform1f("u_useOverrideColor", 1.0f);
    //    // toonShader.SetUniform3f("u_overrideColor", glm::vec3(1.0f, 0.2f, 0.2f));
    //     toonShader.SetUniform3f("u_overrideColor", glm::vec3(1.0f, 0.4f, 0.7f));

        carModel.draw(&toonShader);


        // // Draw car with toon shading
        // float t = time;
        // glm::vec3 carPos = glm::vec3(5.0f * sin(t), -0.4f, 2.5f * sin(t) * cos(t));
        // glm::mat4 carModelMat = glm::mat4(1.0f);
        // carModelMat = glm::translate(carModelMat, carPos);
        // carModelMat = glm::rotate(carModelMat, -t + glm::radians(90.0f), glm::vec3(0,1,0));
        // carModelMat = glm::scale(carModelMat, glm::vec3(0.5f));
        // carModel.update(carModelMat);

        // // Use toon shader for the car
        // toonShader.Bind();
        // toonShader.SetUniform3f("u_lightPos", lightPos);
        // toonShader.SetUniform3f("u_viewPos", camera.getPosition());
        // toonShader.SetUniform4m("u_view", camera.getViewMatrix());
        // toonShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        // toonShader.SetUniform4m("u_model", carModelMat);
        // toonShader.SetUniform1f("u_useOverrideColor", 1.0f);
        // toonShader.SetUniform3f("u_overrideColor", glm::vec3(1.0f, 0.2f, 0.2f));  // Bright cartoon red

        // carModel.draw(&toonShader);
        // shader.SetUniform4m("u_model", carModelMat);
        // shader.SetUniform1f("u_useOverrideColor", 1.0f);
        // shader.SetUniform3f("u_overrideColor", glm::vec3(1.0f, 0.0f, 0.0f));


        // carModel.draw(&shader);
        // shader.SetUniform1f("u_useOverrideColor", 0.0f);
        
        // Draw normal-mapped building
        normalShader.Bind();
        normalShader.SetUniform3f("u_lightPos", lightPos);
        normalShader.SetUniform3f("u_viewPos", camera.getPosition());
        normalShader.SetUniform4m("u_view", camera.getViewMatrix());
        normalShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        normalShader.SetUniform1i("u_useOverrideColor", 0);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseTexture);
        normalShader.SetUniform1i("u_diffuseTexture", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        normalShader.SetUniform1i("u_normalTexture", 1);
        
        glm::mat4 buildingModel = glm::mat4(1.0f);
        buildingModel = glm::translate(buildingModel, glm::vec3(8.0f, 0.0f, -4.0f));
        buildingModel = glm::scale(buildingModel, glm::vec3(1.0f, 5.0f, 1.0f));
        texturedBuilding.update(buildingModel);
        normalShader.SetUniform4m("u_model", buildingModel);
        texturedBuilding.draw(&normalShader);
        
        // Draw sun/light sphere
    //     lightShader.Bind();
    //     lightShader.SetUniform4m("u_view", camera.getViewMatrix());
    //     lightShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
    //     glm::mat4 sunModel = glm::translate(glm::mat4(1.0f), lightPos);
    //     sunModel = glm::scale(sunModel, glm::vec3(0.5f));
    //  //   lightShader.SetUniform4m("u_model", sunModel);
    // //   lightShader.SetUniform3f("u_lightColor", glm::vec3(1.0f, 0.9f, 0.6f) * 0.5f);
    //     lightSphere.draw(&lightShader);
        


        // In render loop, draw sun as billboard
        lightShader.Bind();
        lightShader.SetUniform4m("u_view", camera.getViewMatrix());
        lightShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        lightShader.SetUniform3f("u_sunPos", lightPos);
        lightShader.SetUniform1f("u_size", 0.8f);  // Sun size
        lightShader.SetUniform1f("u_time", time);

        glBindVertexArray(sunVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //draw snow bullbaordi g
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        particleShader.Bind();
        particleShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        particleShader.SetUniform4m("u_view", camera.getViewMatrix());
        particleShader.SetUniform3f("u_cameraRight", camera.getRight());
        particleShader.SetUniform3f("u_cameraUp", camera.getUp());
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, snowflakeTexture);
        particleShader.SetUniform1i("u_particleTexture", 0);
        
        glBindVertexArray(particleVAO);
        for (const auto& particle : snowSystem.getParticles()) {
            if (particle.active) {
                particleShader.SetUniform3f("u_particlePosition", particle.position);
                particleShader.SetUniform1f("u_particleSize", particle.size);
                particleShader.SetUniform1f("u_opacity", particle.opacity);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
        }
        
        snowSystem.update(deltaTime);
        
        glDisable(GL_BLEND);
        
        //draw the skybox last for depth 
        glDepthFunc(GL_LEQUAL);
        skyboxShader.Bind();
        glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
        skyboxShader.SetUniform4m("u_view", view);
        skyboxShader.SetUniform4m("u_projection", camera.getProjectionMatrix());
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        glDepthFunc(GL_LESS);
        
        glfwSwapBuffers(window);
    }
 

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0,0,width,height);
}
