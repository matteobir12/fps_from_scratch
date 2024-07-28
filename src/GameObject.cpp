#include "GameObject.h"
#include "Scene.h"

bool GameObject::isInFieldOfView(const glm::mat4& projectionViewMatrix) {
    std::shared_lock<std::shared_mutex> lock(positionMtx);
    glm::vec4 pos = projectionViewMatrix * glm::vec4(position, 1);
    return pos.x > -1.0f && pos.x < 1.0f && pos.y > -1.0f && pos.y < 1.0f;
}

void GameObject::move(const glm::vec3& offset) {
    std::unique_lock<std::shared_mutex> lock(positionMtx);
    
    position += offset;
}

void GameObject::setPosition(const glm::vec3& newPos) {
    std::unique_lock<std::shared_mutex> lock(positionMtx);
    position = newPos;
}

glm::vec3 GameObject::getPosition() {
    std::shared_lock<std::shared_mutex> lock(positionMtx);
    return position;
}

void GameObject::draw(Camera* const camera) {

    // if (!isInFieldOfView(projectionViewMatrix)) return;
    
    shader->use();
    glBindVertexArray(objData->VAO);
    
    glm::mat4 pvm = camera->getProjectionViewMatrix() * model;
    shader->setUniform("uMat", pvm);
    
    // bind textures, etc.
    for (const GpuGeometry& geom : objData->gpuGeometries) {
        if (geom.material) {
            // Set the diffuse and specular uniform variables
            shader->setUniform("uWorld", model);
            shader->setUniform("uWorldInverseTranspose", modelInverseTranspose);
            shader->setUniform("uViewerWorldPosition", camera->getPosition());

            shader->setUniform("specColor", geom.material->specular);
            shader->setUniform("uShininess", geom.material->specularEx);
            shader->setUniform("lightCutoff", 20.0f); /////// <--------------------------------
            glm::vec3 abl(.75f, .75f, .75f); // hard code, move to map file
            shader->setUniform("ambientLight", abl);
            
            // Bind the texture
            if (geom.material->useTexture) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, geom.material->texture);
                shader->setUniform("uUseTexture", 1);
                shader->setUniform("textureSampler", 0);
            } else {
                shader->setUniform("uUseTexture", 0);
                glm::vec4 color = glm::vec4(geom.material->diffuse, 1.0f);
                shader->setUniform("u_color", color);
            }

        } else {
            shader->setUniform("uUseTexture", 0);
            glm::vec4 color = glm::vec4(0.f, 1.f, 0.f, 1.0f);
            shader->setUniform("u_color", color);
        }

        glDrawArrays(GL_TRIANGLES, geom.offset, geom.size);
        // maybe add dev only env var?
        for (GLenum error = glGetError(); error; error = glGetError()) {
            const auto s = objData->objectId;
            std::cerr << "OpenGL Error in game object when trying to draw " << s << " at draw array (" << error << "): " << std::endl;
        }
    }
}
