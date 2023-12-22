#include "Scene.h"

void Scene::update() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Scene::render() {
    for (GameObject*& object : objects) {
        
        object->draw(camera->getProjectionViewMatrix());
    }
}


// only will need to set the tex uniform and vao once. 
// can cache matrix
// probably only need depthfn call once
void Scene::drawBackground() {
    if (!backgroundProgram || !BackgroundVao) return;

    backgroundProgram->use();

    glBindVertexArray(BackgroundVao);

    glm::mat4 viewMatrix = glm::mat4(camera->getViewMatrix());
    viewMatrix[3][0] = 0;
    viewMatrix[3][1] = 0;
    viewMatrix[3][2] = 0;

    glm::mat4 viewDirectionProjectionMatrix = camera->getProjectionMatrix() * viewMatrix;
    glm::mat4 viewDirectionProjectionInverseMatrix = glm::inverse(viewDirectionProjectionMatrix);

    backgroundProgram->setUniform("uViewDirectionProjectionInverse", viewDirectionProjectionInverseMatrix);
    backgroundProgram->setUniform("uSkybox",0);

    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}