#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "ShaderProgram.h"
#include "SceneLoader.h"
#include "Camera.h"
#include "GameObject.h"
#include "VAOFactory.h"
#include "Scene.h"
#include "HUD.h"
#include "AssetLoader.h"
#include "CommonStructs.h"
#include "GlobalSettings.h"

void GLAPIENTRY MessageCallback( GLenum,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei,
                                 const GLchar* message,
                                 const void* ) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
            << " type = " << type << ", severity = " << severity 
            << ", message = " << message << std::endl;
}


int main() { // int argc, char** argv
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(GlobalSettings::WindowSettings::windowWidth, GlobalSettings::WindowSettings::windowHeight, "My OpenGL Window", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "goodbye world";
        return -1;
    }
    GlobalSettings::NetworkSettings::startNetworkService();

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    ShaderProgram* program = new ShaderProgram("../shaders/no_lighting.vert", "../shaders/no_lighting.frag");
    ShaderProgram* lightProgram = new ShaderProgram("../shaders/scene_object.vert", "../shaders/scene_object.frag");
    ShaderProgram* bp = new ShaderProgram("../shaders/background.vert", "../shaders/background.frag");
    HUD::Initialize(GlobalSettings::WindowSettings::aspectRatio, program);

    Scene s = SceneLoader::loadFromFile(GlobalSettings::AssetSettings::mapDir.string() + "test.mp", bp, lightProgram, program);
    s.setLightProgram(lightProgram);
    s.setServerThread(nullptr);
    glfwSetWindowUserPointer(window, &s);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            Scene* myObject = static_cast<Scene*>(glfwGetWindowUserPointer(window));
            if (myObject->firstMouse) {
                myObject->lastX = xpos;
                myObject->lastY = ypos;
                myObject->firstMouse = false;
            }

            float xoffset = xpos - myObject->lastX;
            float yoffset = myObject->lastY - ypos;

            myObject->lastX = xpos;
            myObject->lastY = ypos;

            myObject->camera->rotate(xoffset, yoffset);
    });

    glfwSetKeyCallback(window, Scene::key_callback);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        s.update(window);
        s.render();

        glfwSwapBuffers(window);

        glfwPollEvents();
        
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

/*
for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << viewMatrix[j][i] << " ";
            }
            std::cout << std::endl;
        }
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                std::cout << projectionMatrix[j][i] << " ";
            }
            std::cout << std::endl;
        }*/



