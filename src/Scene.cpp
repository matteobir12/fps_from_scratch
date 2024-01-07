#include "Scene.h"

void Scene::update() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
}

void Scene::render() {
    drawBackground();
    for (GameObject*& object : objects) {
        
        object->draw(camera->getProjectionViewMatrix());
    }
}

Scene::Scene(Camera* camera, const std::vector<GameObject*>& objects, ShaderProgram* backgroundProgram, GLuint backgroundTex) 
: objects(objects), backgroundProgram(backgroundProgram), backgroundTex(backgroundTex), camera(camera) {
    
    GLuint backVertexBuffer;

    glGenVertexArrays(1, &backgroundVao);
    glBindVertexArray(backgroundVao);

    glGenBuffers(1, &backVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, backVertexBuffer);

    std::vector<float> backgroundPositions = BackgroundPositions();
    glBufferData(GL_ARRAY_BUFFER, backgroundPositions.size() * sizeof(float), backgroundPositions.data(), GL_STATIC_DRAW);

    // Can abstract
    GLuint backaPositionLocation = glGetAttribLocation(backgroundProgram->getID(), "aPosition");

    glEnableVertexAttribArray(backaPositionLocation);
    glVertexAttribPointer(backaPositionLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

    backgroundProgram->setUniform("uSkybox",0);


 
}


// only will need to set vao once. 
// can cache matrix
// probably only need depthfn call once
void Scene::drawBackground() {
    if (!backgroundProgram || !backgroundVao) return;

    backgroundProgram->use();

    glBindVertexArray(backgroundVao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, backgroundTex);

    glm::mat4 viewMatrix = glm::mat4(camera->getViewMatrix());
    viewMatrix[3][0] = 0;
    viewMatrix[3][1] = 0;
    viewMatrix[3][2] = 0;

    glm::mat4 viewDirectionProjectionMatrix = camera->getProjectionMatrix() * viewMatrix;
    glm::mat4 viewDirectionProjectionInverseMatrix = glm::inverse(viewDirectionProjectionMatrix);

    backgroundProgram->setUniform("uViewDirectionProjectionInverse", viewDirectionProjectionInverseMatrix);


    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}