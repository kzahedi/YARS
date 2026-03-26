#include "VulkanCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace yars {

Camera::Camera()
    : _position(0.0f, 0.0f, 3.0f)
    , _target(0.0f, 0.0f, 0.0f)
    , _up(0.0f, 1.0f, 0.0f)
    , _projectionMatrix(glm::mat4(1.0f)) {
}

void Camera::setPosition(const glm::vec3& position) {
    _position = position;
}

void Camera::setTarget(const glm::vec3& target) {
    _target = target;
}

void Camera::setUp(const glm::vec3& up) {
    _up = up;
}

void Camera::setPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane) {
    _projectionMatrix = glm::perspective(glm::radians(fovY), aspectRatio, nearPlane, farPlane);
    // GLM was designed for OpenGL, where Y coordinate of clip coordinates is inverted
    // For Vulkan, we need to flip Y
    _projectionMatrix[1][1] *= -1;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    _projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    // Flip Y for Vulkan
    _projectionMatrix[1][1] *= -1;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(_position, _target, _up);
}

glm::mat4 Camera::getViewProjectionMatrix() const {
    return _projectionMatrix * getViewMatrix();
}

void Camera::moveForward(float distance) {
    glm::vec3 forward = glm::normalize(_target - _position);
    _position += forward * distance;
    _target += forward * distance;
}

void Camera::moveRight(float distance) {
    glm::vec3 forward = glm::normalize(_target - _position);
    glm::vec3 right = glm::normalize(glm::cross(forward, _up));
    _position += right * distance;
    _target += right * distance;
}

void Camera::moveUp(float distance) {
    _position += _up * distance;
    _target += _up * distance;
}

void Camera::rotate(float yaw, float pitch) {
    glm::vec3 direction = _target - _position;
    float distance = glm::length(direction);

    // Convert to spherical coordinates
    float currentYaw = atan2(direction.z, direction.x);
    float currentPitch = asin(direction.y / distance);

    // Apply rotation
    currentYaw += glm::radians(yaw);
    currentPitch += glm::radians(pitch);

    // Clamp pitch to avoid gimbal lock
    currentPitch = glm::clamp(currentPitch, glm::radians(-89.0f), glm::radians(89.0f));

    // Convert back to Cartesian
    direction.x = cos(currentPitch) * cos(currentYaw);
    direction.y = sin(currentPitch);
    direction.z = cos(currentPitch) * sin(currentYaw);

    _target = _position + glm::normalize(direction) * distance;
}

void Camera::lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) {
    _position = position;
    _target = target;
    _up = up;
}

} // namespace yars
