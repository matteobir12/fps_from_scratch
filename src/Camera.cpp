#include "Camera.h"

Camera::Camera(const glm::vec3& position, const glm::vec3& target, float fov, float aspectRatio)
    : position(position), target(target), fov(fov), aspectRatio(aspectRatio) {
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetPosition(const glm::vec3& newPosition) {
    position = newPosition;
    UpdateViewMatrix();
}

void Camera::LookAt(const glm::vec3& target) {
    this->target = target;
    UpdateViewMatrix();
}

void Camera::SetFOV(float newFOV) {
    fov = newFOV;
    UpdateProjectionMatrix();
}

void Camera::SetAspectRatio(float newAspectRatio) {
    aspectRatio = newAspectRatio;
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix() {
    viewMatrix = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
    didUpdate = true;
}

void Camera::UpdateProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 1000.0f);
    didUpdate = true;
}

void Camera::Move(const glm::vec3& offset) {
    glm::translate(viewMatrix,offset);
    didUpdate = true;
}

void Camera::Rotate(float deltaYaw, float deltaPitch) {
    glm::mat4 rotationMatrix = viewMatrix;
    glm::vec3 translation = glm::vec3(rotationMatrix[3]);
    rotationMatrix[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(deltaYaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(deltaPitch), glm::vec3(1.0f, 0.0f, 0.0f));

    rotationMatrix = yawMatrix * rotationMatrix;
    rotationMatrix = rotationMatrix * pitchMatrix;

    viewMatrix = rotationMatrix;
    viewMatrix[3] = glm::vec4(translation, 1.0f);
    didUpdate = true;
}

const glm::mat4& Camera::GetViewProjectionMatrix(){
    if (didUpdate){
        viewProjectionMatrix = viewMatrix * projectionMatrix;
        didUpdate = false;
    }
    return viewProjectionMatrix;
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
