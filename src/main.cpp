#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "ShaderProgram.h"
#include "Camera.h"
#include "GameObject.h"
#include "VAOFactory.h"
#include "Scene.h"
#include "AssetLoader.h"

void GLAPIENTRY MessageCallback( GLenum source,
                                 GLenum type,
                                 GLuint id,
                                 GLenum severity,
                                 GLsizei length,
                                 const GLchar* message,
                                 const void* userParam ) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
            << " type = " << type << ", severity = " << severity 
            << ", message = " << message << std::endl;
}


int main() { // int argc, char** argv
    int HEIGHT = 800;
    int WIDTH = 1000;
    int FOV = 45;
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "My OpenGL Window", NULL, NULL);
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
    std::vector<float> vertices = {
        // Front face
        -0.5f, -0.5f,  0.5f, // Bottom Left
        0.5f, -0.5f,  0.5f, // Bottom Right
        0.5f,  0.5f,  0.5f, // Top Right
        -0.5f,  0.5f,  0.5f, // Top Left
        //back
        -0.5f, -0.5f,  -0.5f, // Bottom Left
        0.5f, -0.5f,  -0.5f, // Bottom Right
        0.5f,  0.5f,  -0.5f, // Top Right
        -0.5f,  0.5f,  -0.5f, // Top Left

    };
    std::vector<unsigned int> indices = {
        // front face
        0, 1, 2,
        2, 3, 0,
        // left face
        0, 3, 4,
        3, 7, 4,
        // right face
        1, 2, 4,
        5, 6, 2,
        // back face
        4, 5, 6,
        6, 7, 4,
        // top face
        2, 3, 7,
        6, 7, 2,
        // bottom
        4, 5, 1,
        1, 0, 4,
    };

       std::vector<std::string> faces{
        "./textures/background/pos-x.jpg",
        "./textures/background/neg-x.jpg",
        "./textures/background/pos-y.jpg",
        "./textures/background/neg-y.jpg",
        "./textures/background/pos-z.jpg",
        "./textures/background/neg-z.jpg"
    };

    GLuint cubemapTexture = AssetLoader::createCubeMap(faces);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    std::vector<GLuint> glbuffs = VAOFactory::createVAO(vertices, &indices);
    std::string vertexShaderPath = "../shaders/no_lighting.vert";
    std::string fragmentShaderPath = "../shaders/no_lighting.frag";
    ShaderProgram* program = new ShaderProgram(vertexShaderPath, fragmentShaderPath);
    ShaderProgram* bp = new ShaderProgram("../shaders/background.vert", "../shaders/background.frag");
    GameObject* object = new GameObject(program, glbuffs, 80, glm::vec3(-10,0,-10),glm::vec3(1, 1, 1),glm::vec3(0, 0, 0));
    GameObject* plane = new GameObject(program, glbuffs, 80, glm::vec3(0,0,-2),glm::vec3(100, 100, .1),glm::vec3(0, 0, 0));
    Camera* c = new Camera(glm::vec3(0,-2,0),glm::vec3(-1,-1,-1),FOV, 800.0f/600.0f);
    std::vector<GameObject*> objs = { 
        object,
        plane,
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,-10,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(0,-10,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,0,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,-10,0)),
        // new GameObject(program, glbuffs, 10, glm::vec3(10,10,10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(0,-10,0)),
     };
    Scene s = Scene(c,objs,bp,cubemapTexture);
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
        glClear(GL_COLOR_BUFFER_BIT);

        s.update();
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



