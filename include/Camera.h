#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewProjectionMatrix;

    glm::vec3 position;
    glm::vec3 target;
    float fov;
    float aspectRatio;
    bool didUpdate;

    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

public:
    Camera(const glm::vec3& position, const glm::vec3& target, float fov, float aspectRatio);

    const glm::mat4& GetViewMatrix() const { return viewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }
    const glm::mat4& GetViewProjectionMatrix();

    void SetPosition(const glm::vec3& newPosition);
    void LookAt(const glm::vec3& target);

    void SetFOV(float newFOV);
    void SetAspectRatio(float newAspectRatio);

    void Move(const glm::vec3& offset);
    void Rotate(float deltaYaw, float deltaPitch);

};

#endif