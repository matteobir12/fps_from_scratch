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