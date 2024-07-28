#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <mutex>
#include <shared_mutex>

#include <glm/gtc/matrix_transform.hpp>

#include "ShaderProgram.h"
#include "CommonStructs.h"
#include "PhysicsObject.h"
#include "Camera.h"

using namespace Rendering;

class GameObject {
    private:
        ShaderProgram* shader;
        bool usesLight;
        glm::mat4 model;
        bool modelDidUpdate;
        glm::mat4 modelInverseTranspose;
        glm::vec3 position;
        mutable std::shared_mutex positionMtx;
        glm::vec3 scale;
        mutable std::shared_mutex scaleMtx;
        glm::vec3 rotation;
        mutable std::shared_mutex rotationMtx;
        const GpuObject* objData;

        bool isInFieldOfView(const glm::mat4& projectionViewMatrix);

        // can maybe add caching or other stuff if for some reason this gets slow
        void updateModel() {
            std::shared_lock<std::shared_mutex> lock1(scaleMtx);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
            lock1.release();

            std::shared_lock<std::shared_mutex> lock2(rotationMtx);
            glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            lock2.release();
            glm::mat4 rotationMat = rotX * rotY * rotZ;

            std::shared_lock<std::shared_mutex> lock(positionMtx);
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), position);
            lock.release();

            model = translationMat * rotationMat * scaleMat;
            modelInverseTranspose = glm::transpose(glm::inverse(model));
        }

    public:
        const PhysicsObject * physData = nullptr;

        /**
         * @brief Construct a new Game Object object
         * 
         * Remove all these pointers 
         * 
         * @param shaderProgram 
         * @param objData 
         * @param prs 
         * @param physData 
         * @param flags 
         */
        GameObject(
            ShaderProgram* shaderProgram,
            const GpuObject* objData,
            const PositionRotationScale& prs = {},
            const PhysicsObject *physData = nullptr,
            const GameObjectFlags& flags = {})
            : shader(shaderProgram),
                modelDidUpdate(false),
                position(prs.objectPosition),
                scale(prs.objectScale),
                rotation(prs.objectRotation),
                objData(objData),
                physData(physData) {
            updateModel();
            usesLight = flags.usesLight;
        }

        void draw(Camera* const camera);

        void move(const glm::vec3& offset);

        void setPosition(const glm::vec3& newPos);

        glm::vec3 getPosition();

};

#endif