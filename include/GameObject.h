#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "CommonStructs.h"
#include <mutex>
#include <shared_mutex>



class GameObject {
    private:
        ShaderProgram* shader;
        glm::mat4 model;
        bool modelDidUpdate;
        glm::mat4 modelInverseTranspose;
        glm::vec3 position;
        mutable std::shared_mutex positionMtx;
        glm::vec3 scale;
        mutable std::shared_mutex scaleMtx;
        glm::vec3 rotation;
        mutable std::shared_mutex rotationMtx;
        GpuObject* objData;

        bool isInFieldOfView(const glm::mat4& projectionViewMatrix);

        // can maybe add caching or other stuff if for some reason this gets slow
        void updateModel() {
            std::shared_lock<std::shared_mutex> lock(positionMtx);
            std::shared_lock<std::shared_mutex> lock1(scaleMtx);
            std::shared_lock<std::shared_mutex> lock2(rotationMtx);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);

            glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            glm::mat4 rotationMat = rotX * rotY * rotZ;
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);

            model = translationMat * rotationMat * scaleMat;
            modelInverseTranspose = glm::transpose(glm::inverse(model));
        }
    public:

        GameObject(ShaderProgram* shaderProgram, GpuObject* objData, const glm::vec3& objectPosition = glm::vec3(), const glm::vec3& objectScale = glm::vec3(1.0f), const glm::vec3& objectRotation = glm::vec3())
        : shader(shaderProgram), modelDidUpdate(false), position(objectPosition), scale(objectScale), rotation(objectRotation), objData(objData) {
            updateModel();
        }

        void draw(const glm::mat4& projectionViewMatrix);

        void move(glm::vec3& offset);

        void setPosition(glm::vec3& newPos);

        glm::vec3 getPosition();

};

#endif