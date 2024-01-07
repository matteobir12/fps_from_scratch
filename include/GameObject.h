#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "CommonStructs.h"


class GameObject {
    private:
        ShaderProgram* shader;
        glm::mat4 model;
        glm::mat4 modelInverseTranspose;
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
        GpuObject* objData;

        bool isInFieldOfView(const glm::mat4& projectionViewMatrix);

        void updateModelInverseTranspose() {
            modelInverseTranspose = glm::transpose(glm::inverse(model));
        }
    public:
        GameObject(ShaderProgram* shaderProgram, GpuObject* objData) : shader(shaderProgram), objData(objData) { 
            model = glm::mat4(1);
            updateModelInverseTranspose();
        }

        GameObject(ShaderProgram* shaderProgram, GpuObject* objData, const glm::vec3& objectPosition) : shader(shaderProgram), objData(objData) {
            model = glm::translate(glm::mat4(1), objectPosition);
            updateModelInverseTranspose();
        }

        GameObject(ShaderProgram* shaderProgram, GpuObject* objData, const glm::vec3& objectPosition, const glm::vec3& objectScale, const glm::vec3& objectRotation);

        const glm::vec4& readOnlyPositionHomo() const { return model[3]; }

        void draw(const glm::mat4& projectionViewMatrix);

};

#endif