#include "Camera.h"

Camera::Camera(const glm::vec3 position, float fov, float aspectRatio, glm::vec3 startUp, float startYaw, float startPitch)
    : position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(startUp), yaw(startYaw), pitch(startPitch), fov(fov), aspectRatio(aspectRatio) {
    mouseSensitivity = 1;
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    updateViewMatrix();
}

void Camera::setFOV(float newFOV) {
    fov = newFOV;
    updateProjectionMatrix();
}

void Camera::setAspectRatio(float newAspectRatio) {
    aspectRatio = newAspectRatio;
    updateProjectionMatrix();
}

void Camera::updateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::updateProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
    didUpdate = true;
}

void Camera::translateRelative(const glm::vec3& offset) {
    position += right * offset.x;
    position += up * offset.y;
    position += front * offset.z; // move speed
    didUpdate = true;
}

void Camera::translateGame(const glm::vec3& offset) {
    position += right * offset.x;
    position.y += offset.y;

    glm::vec3 forwardOnPlane = glm::normalize(glm::vec3(front.x, 0.0f, front.z));
    position += forwardOnPlane * offset.z;

    didUpdate = true;
}

void Camera::translate(const glm::vec3& offset) {
    position += offset;
    didUpdate = true;
}

const glm::mat4& Camera::getProjectionViewMatrix(){
    if (didUpdate){
        updateViewMatrix();
        projectionViewMatrix = projectionMatrix * viewMatrix;
        didUpdate = false;
    }
    return projectionViewMatrix;
}

void Camera::rotate(float xoffset, float yoffset, bool constrainPitch) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

void Camera::updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));  
        up    = glm::normalize(glm::cross(right, front));
        didUpdate = true;
}

// class Camera {
//     private:
//         glm::mat4 viewMatrix;
//         glm::mat4 projectionMatrix;

//         glm::vec3 position;
//         float fov;
//         float aspectRatio;

//     public:
//         Camera::Camera(const glm::vec3& position, float yaw, float pitch, float fov, float aspectRatio)
//         : position(position), yaw(yaw), pitch(pitch), fov(fov), aspectRatio(aspectRatio) {
//         UpdateCameraVectors();
//         }

//         glm::mat4 Camera::GetViewMatrix() const {
//             glm::vec3 front;
//             front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//             front.y = sin(glm::radians(pitch));
//             front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//             front = glm::normalize(front);

//             return glm::lookAt(position, position + front, glm::vec3(0.0f, 1.0f, 0.0f));
//         }

//         glm::mat4 GetProjectionMatrix() const {
//         return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
//         }

//         void Camera::Move(const glm::vec3& offset) {
//             position += offset;
//         }

//         void Camera::Rotate(float deltaYaw, float deltaPitch) {
//             yaw += deltaYaw;
//             pitch += deltaPitch;
            
//             // might constrain pitch
//             // pitch = glm::clamp(pitch, -89.0f, 89.0f);

//             UpdateCameraVectors();
//         }
// };
