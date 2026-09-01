#pragma once
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <iostream>

#ifndef CAMERA_H
#define CAMERA_H

// Forward declaration
class BezierCurve;

class Camera {
private:
    GLFWwindow* window;
    glm::vec3 carTarget;
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    
    float yaw;
    float pitch;
    float sens;
    float spd;
    bool useOrtho;
    bool useLookAt;
    glm::vec3 lookAtTarget;
    
    // Path following variables
    bool followPath;
    BezierCurve* path;
    float pathProgress;
    
public:

//setter methods
    void setCarTarget(const glm::vec3& target);
    void setPosition(const glm::vec3& pos) { position = pos; }
    void setForward(const glm::vec3& fwd) { 
        forward = glm::normalize(fwd);
        // Recalculate right and up vectors
        right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
        up = glm::normalize(glm::cross(right, forward));
    }
    

   
    Camera(GLFWwindow* window, float sensitivity, float speed);
    ~Camera();
    
    void toggleProjection();
    void update(float deltaTime);
    void lookAt(const glm::vec3& target);
    void clearLookAt();
    
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    void sendToShader(Shader* shader);
    
    // Getters
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getForward() const { return forward; }
    glm::vec3 getRight() const { return right; }
    glm::vec3 getUp() const { return up; }
    
    // Path following methods
    void setPath(BezierCurve* newPath);
    void updatePath(float deltaTime, float speed = 0.2f);
    void disablePathFollowing();
    void enablePathFollowing();
};

#endif