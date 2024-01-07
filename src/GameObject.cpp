#include "GameObject.h"


bool GameObject::isInFieldOfView(const glm::mat4& projectionViewMatrix) {
    glm::vec4 pos = projectionViewMatrix * readOnlyPositionHomo();
    return pos.x > -1.0f && pos.x < 1.0f && pos.y > -1.0f && pos.y < 1.0f;
}


GameObject::GameObject(ShaderProgram* shaderProgram, GpuObject* objData, const glm::vec3& objectPosition, const glm::vec3& objectScale, const glm::vec3& objectRotation) :
    shader(shaderProgram), objData(objData)  {
    model = glm::mat4(1);
    model = glm::rotate(model, objectRotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, objectRotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, objectRotation.z, glm::vec3(0, 0, 1));

    model = glm::scale(model, objectScale);


    model = glm::translate(model,objectPosition);
    // for(auto c : objData->gpuGeometries){
    //     std::cout << "x:" <<c.material<< std::endl;
    // }
    updateModelInverseTranspose();
}

void GameObject::draw(const glm::mat4& projectionViewMatrix) {

    // if (!isInFieldOfView(projectionViewMatrix)) return;
    
    shader->use();
    glBindVertexArray(objData->VAO);
    
    glm::mat4 pvm = projectionViewMatrix * model;
    shader->setUniform("uMat", pvm);
    
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    shader->setUniform("u_color", color);
    // bind textures, etc.
    for (const GpuGeometry& geom : objData->gpuGeometries) {
        if (geom.material) {
            // Set the diffuse and specular uniform variables
            // glUniform3fv(diffuseUniformLocation, 1, glm::value_ptr(geom.material->diffuse));
            // glUniform3fv(specularUniformLocation, 1, glm::value_ptr(geom.material->specular));
            // glUniform1f(specularExUniformLocation, geom.material->specularEx);

            shader->setUniform("uWorldInverseTranspose", modelInverseTranspose);

            shader->setUniform("specColor", geom.material->specular);
            shader->setUniform("uShininess", geom.material->specularEx);
            glm::vec3 abl(1.0f, 1.0f, 1.0f);
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
        }

        glDrawArrays(GL_TRIANGLES, geom.offset, geom.size);
        // maybe add dev only env var?
        for (GLenum error = glGetError(); error; error = glGetError()) {
            std::cerr << "OpenGL Error at draw array (" << error << "): " << std::endl;
        }
    }
}


// Setters and getters for position, scale, rotation, etc.
