#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Networking/ServerThread.h"
#include "Camera.h"
#include "GameObject.h"
#include "ShaderProgram.h"
#include "GlobalSettings.h"


class Scene {
    private:
        std::vector<GameObject*> objects;
        GameObject* player_model = nullptr;
        ServerThread* server = nullptr;
        ShaderProgram* backgroundProgram = nullptr;
        ShaderProgram* lightProgram = nullptr;
        GLuint backgroundVao;
        GLuint backgroundTex;
        bool drawHud = true;
        // todo window class
        std::vector<float> BackgroundPositions();
        void setLightUniforms();
        
    public:
        std::vector<Light> sceneLights{}; // change to faster fit lights
        double lastX = GlobalSettings::WindowSettings::windowWidth / 2, lastY = GlobalSettings::WindowSettings::windowHeight / 2;
        Camera* camera;
        bool firstMouse = true;

        Scene(Camera* camera, std::vector<GameObject*> objects) : objects(std::move(objects)), camera(camera) {}
        Scene(Camera* camera, std::vector<GameObject*> objects, GameObject* player_model, ShaderProgram* backgroundProgram, GLuint backgroundTex);

        void update(GLFWwindow* window);

        void render();

        void drawBackground();

        void drawPlayers();

        void setServerThread(ServerThread* thread) {
            server = thread;
        }

        void setLightProgram(ShaderProgram* lp) {
            lightProgram = lp;
        }

        static void key_callback(GLFWwindow* window, int key, int, int action, int mods) {
            Scene* myObject = static_cast<Scene*>(glfwGetWindowUserPointer(window));
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }

            if (key == GLFW_KEY_P && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods & GLFW_MOD_CONTROL) {
                glm::mat4 proj = myObject->camera->getViewMatrix();
                for (int i = 0; i < 3; i++){
                    std::cout << proj[3][i] << " ";
                }
                std::cout << std::endl;
            }
            if (key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT) && mods & GLFW_MOD_CONTROL) {
                myObject->drawHud = !myObject->drawHud;
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