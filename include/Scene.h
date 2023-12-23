#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "Camera.h"
#include "GameObject.h"
#include "ShaderProgram.h"

class Scene {
    private:
        std::vector<GameObject*> objects;
        ShaderProgram* backgroundProgram = nullptr;
        GLuint backgroundVao;
        GLuint backgroundTex;
        // todo window class
        std::vector<float> BackgroundPositions();

    public:
        double lastX = 400, lastY = 300;
        Camera* camera;
        bool firstMouse = true;

        Scene(Camera* camera, const std::vector<GameObject*>& objects) : objects(objects), camera(camera) {}
        Scene(Camera* camera, const std::vector<GameObject*>& objects, ShaderProgram* backgroundProgram, GLuint backgroundTex);

        void update();

        void render();

        void drawBackground();

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            Scene* myObject = static_cast<Scene*>(glfwGetWindowUserPointer(window));
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if (key == GLFW_KEY_W){
                myObject->camera->translate(glm::vec3(1,0,0));
            }
            if (key == GLFW_KEY_S){
                myObject->camera->translate(glm::vec3(-1,0,0));
            }
            if (key == GLFW_KEY_D){
                myObject->camera->translate(glm::vec3(0,-1,0));
            }
            if (key == GLFW_KEY_A){
                myObject->camera->translate(glm::vec3(0,1,0));
            }
            if (key == GLFW_KEY_SPACE){
                myObject->camera->translate(glm::vec3(0,0,1));
            }
            if (key == GLFW_KEY_LEFT_SHIFT){
                myObject->camera->translate(glm::vec3(0,0,-1));
            }
            if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods & GLFW_MOD_CONTROL) {
                glm::mat4 proj = myObject->camera->getViewMatrix();
                for (int i = 0; i < 3; i++){
                    std::cout << proj[3][i] << " ";
                }
                std::cout << std::endl;
            }
            if (key == GLFW_KEY_M && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods & GLFW_MOD_CONTROL) {
                GLint maxFragmentTextureUnits;
                glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentTextureUnits);

                GLint maxCombinedTextureUnits;
                glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits);

                std::cout << "Maximum Fragment Texture Units: " << maxFragmentTextureUnits << std::endl;
                std::cout << "Maximum Combined Texture Units: " << maxCombinedTextureUnits << std::endl;
            }


        }


};

#endif