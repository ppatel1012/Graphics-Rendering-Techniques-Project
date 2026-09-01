#include "Camera.h"
#include "BezierCurve.h" 
#include <iostream>

Camera::Camera(GLFWwindow* window, float sensitivity, float speed)
{
    this->window = window;
    position = glm::vec3(0.0f, 0.0f, 3.0f);

    yaw   = -90.0f; 
    pitch = 0.0f;

    sens = sensitivity;
    spd  = speed;

    // initialize vectors
    forward = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::vec3(1.0f, 0.0f,  0.0f);
    up      = glm::vec3(0.0f, 1.0f,  0.0f);

    useOrtho = false;

    followPath = false;
    path = nullptr;
    pathProgress = 0.0f;
}

Camera::~Camera() {}

void Camera::toggleProjection()
{
    useOrtho = !useOrtho;
}
void Camera::setCarTarget(const glm::vec3& target) {
    carTarget = target;
}

void Camera::update(float deltaTime) {
    // If following a path, use automatic movement
    if (followPath && path != nullptr) {
        updatePath(deltaTime, 0.06f);  // Speed = 0.2 
        return;
    }
    
    // // Otherwise, use keyboard controls (original behavior)
    float speed = spd * deltaTime;
    float sensitivity = sens * deltaTime;

    // Arrow keys rotate not move
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        yaw -= sensitivity;

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        yaw += sensitivity;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        pitch += sensitivity;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        pitch -= sensitivity;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward.y = sin(glm::radians(pitch));
    forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    forward = glm::normalize(forward);

    right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, forward));

    // AWSDQE move 
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += forward * speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= forward * speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= right * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += right * speed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) position -= glm::vec3(0.0f,1.0f,0.0f)*speed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) position += glm::vec3(0.0f,1.0f,0.0f)*speed;
}

void Camera::setPath(BezierCurve* newPath) {
    path = newPath;
    followPath = true;
    pathProgress = 0.0f;
}

// void Camera::updatePath(float deltaTime, float speed) {
//     if (!followPath || path == nullptr) return;
    
//     // Progress continues infinitely
//     pathProgress += deltaTime * speed;
    
//     // Get fractional part (0 to 1)
//     float t = pathProgress - floor(pathProgress);
    
//     // Get position on curve
//     position = path->getPosition(t);
    
//     // Get direction from curve tangent (this is naturally smooth!)
//     glm::vec3 targetForward = path->getTangent(t);
    
//     // Smooth rotation with very slow interpolation
//     float lerpFactor = 0.08f;
//     forward = glm::normalize(forward * (1.0f - lerpFactor) + targetForward * lerpFactor);
    
//     // Update orientation
//     up = glm::vec3(0.0f, 1.0f, 0.0f);
//     right = glm::normalize(glm::cross(forward, up));
// }


glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + forward, up);
}

//perspective projection matrix
glm::mat4 Camera::getProjectionMatrix()
{
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    if (windowHeight == 0) windowHeight = 1;
    float aspect = (float)windowWidth / (float)windowHeight;

    if (useOrtho)
    {
        float zoom = 1.0f;
        return glm::ortho(
            -zoom * aspect, zoom * aspect,
            -zoom, zoom,
            0.1f, 100.0f
        );
    }

    return glm::perspective(
        glm::radians(45.0f),
        aspect,
        0.1f,
        100.0f
    );
}

void Camera::sendToShader(Shader* shader)
{
    shader->SetUniform4m("u_view", getViewMatrix());
    shader->SetUniform4m("u_projection", getProjectionMatrix());
}

void Camera::lookAt(const glm::vec3& target) {
    useLookAt = true;
    lookAtTarget = target;
}

void Camera::clearLookAt() {
    useLookAt = false;
}

void Camera::updatePath(float deltaTime, float speed) {
    if (!followPath || path == nullptr) return;
    
    // Progress continues infinitely
    pathProgress += deltaTime * speed;
    float t = pathProgress - floor(pathProgress);
    
    // Get position on curve
    position = path->getPosition(t);
    
    // Get curve direction (normal path following)
    glm::vec3 curveForward = path->getTangent(t);
    
    // Get look-at direction (toward car)
    glm::vec3 lookAtForward = glm::normalize(carTarget - position);
    
    // Calculate blend factor based on height (smooth transition)
    float blend = 0.0f;
    if (position.y > 6.0f) {
        // Smooth blend from 0 to 1 between Y=6 and Y=10
        blend = glm::clamp((position.y - 6.0f) / 4.0f, 0.0f, 1.0f);
    }
    
    // Blend between curve direction and look-at direction
    glm::vec3 targetForward = glm::normalize(
        curveForward * (1.0f - blend) + lookAtForward * blend
    );
    
    // Smooth rotation
    float lerpFactor = 0.01f;
    forward = glm::normalize(forward * (1.0f - lerpFactor) + targetForward * lerpFactor);
    
    // Update orientation
    up = glm::vec3(0.0f, 1.0f, 0.0f);
    right = glm::normalize(glm::cross(forward, up));
}
