#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 projectionViewMatrix;

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    float mouseSensitivity;

    float yaw;
    float pitch;

    float fov;
    float aspectRatio;
    bool didUpdate;

    void updateViewMatrix();
    void updateProjectionMatrix();

public:
    Camera(const glm::vec3 position, float fov, float aspectRatio, glm::vec3 startUp, float startYaw, float startPitch);

    const glm::mat4& getViewMatrix() const { return viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
    const glm::mat4& getProjectionViewMatrix();

    void setPosition(const glm::vec3& newPosition);

    void setFOV(float newFOV);
    void setAspectRatio(float newAspectRatio);

    void move(const glm::vec3& offset);
    void translate(const glm::vec3& offset);

    void updateCameraVectors();
    void rotate(float xoffset, float yoffset, bool constrainPitch = true);

};

#endif