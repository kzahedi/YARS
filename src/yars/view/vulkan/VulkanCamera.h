#ifndef __VULKAN_CAMERA_H__
#define __VULKAN_CAMERA_H__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace yars {

/**
 * Camera system for 3D viewing
 */
class Camera {
public:
    Camera();

    // Camera positioning
    void setPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setUp(const glm::vec3& up);

    glm::vec3 getPosition() const { return _position; }
    glm::vec3 getTarget() const { return _target; }
    glm::vec3 getUp() const { return _up; }

    // Projection
    void setPerspective(float fovY, float aspectRatio, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    // Matrix getters
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const { return _projectionMatrix; }
    glm::mat4 getViewProjectionMatrix() const;

    // Camera movement helpers
    void moveForward(float distance);
    void moveRight(float distance);
    void moveUp(float distance);
    void rotate(float yaw, float pitch);
    void lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);

private:
    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _up;

    glm::mat4 _projectionMatrix;

    void updateVectors();
};

/**
 * Uniform buffer object for MVP matrices
 */
struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

} // namespace yars

#endif // __VULKAN_CAMERA_H__
