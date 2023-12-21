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
    glm::vec3 target;
    float fov;
    float aspectRatio;
    bool didUpdate;

    void updateViewMatrix();
    void updateProjectionMatrix();

public:
    Camera(const glm::vec3& position, const glm::vec3& target, float fov, float aspectRatio);

    const glm::mat4& getViewMatrix() const { return viewMatrix; }
    const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
    const glm::mat4& getProjectionViewMatrix();

    void setPosition(const glm::vec3& newPosition);
    void lookAt(const glm::vec3& target);

    void setFOV(float newFOV);
    void setAspectRatio(float newAspectRatio);

    void move(const glm::vec3& offset);
    void translate(const glm::vec3& offset);
    void rotate(float deltaYaw, float deltaPitch);

};

#endif