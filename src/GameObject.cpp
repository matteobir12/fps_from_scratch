#include "GameObject.h"


bool GameObject::isInFieldOfView(const glm::mat4& projectionViewMatrix) {
    glm::vec4 pos = projectionViewMatrix * readOnlyPositionHomo();
    return pos.x > -1.0f && pos.x < 1.0f && pos.y > -1.0f && pos.y < 1.0f;
}


GameObject::GameObject(ShaderProgram* shaderProgram, std::vector<GLuint>& buffers, unsigned int triangleCount, const glm::vec3& objectPosition, const glm::vec3& objectScale, const glm::vec3& objectRotation) :
    shader(shaderProgram), buffers(buffers), count(triangleCount)  {
    model = glm::mat4(1);
    model = glm::rotate(model, objectRotation.x, glm::vec3(1, 0, 0));
    model = glm::rotate(model, objectRotation.y, glm::vec3(0, 1, 0));
    model = glm::rotate(model, objectRotation.z, glm::vec3(0, 0, 1));

    model = glm::scale(model, objectScale);


    model = glm::translate(model,objectPosition);
}

void GameObject::draw(const glm::mat4& projectionViewMatrix) {

    // if (!isInFieldOfView(projectionViewMatrix)) return;
    shader->use();
    glBindVertexArray(buffers[0]);
    // Set uniforms (like model matrix, view-projection matrix, etc.)
    
    glm::mat4 mvp = projectionViewMatrix * model;
    shader->setUniform("uMat", mvp);
    glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    shader->setUniform("u_color", color);

    // bind textures, etc.
    if (buffers[2]){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
        for (GLenum error = glGetError(); error; error = glGetError()) {
            std::cerr << "OpenGL Error (" << error << "): " << std::endl;
        }
    } else {
        std::cout << "stinky";
    }
    
    // add support for draw arrays
}


// Setters and getters for position, scale, rotation, etc.
