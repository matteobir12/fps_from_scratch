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
#include "CommonStructs.h"

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
    int HEIGHT = 800.0f;
    int WIDTH = 1000.0f;
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
        "background/pos-x.jpg",
        "background/neg-x.jpg",
        "background/pos-y.jpg",
        "background/neg-y.jpg",
        "background/pos-z.jpg",
        "background/neg-z.jpg"
    };

    GLuint cubemapTexture = AssetLoader::createCubeMap(faces);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    std::string vertexShaderPath = "../shaders/no_lighting.vert";
    std::string fragmentShaderPath = "../shaders/no_lighting.frag";
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    ShaderProgram* program = new ShaderProgram(vertexShaderPath, fragmentShaderPath);
    ShaderProgram* lightProgram = new ShaderProgram("../shaders/scene_object.vert", "../shaders/scene_object.frag");
    ShaderProgram* bp = new ShaderProgram("../shaders/background.vert", "../shaders/background.frag");
    // tmp cube stuff
    std::string vaoid = "cube";
    CpuGeometry* cpuCube = new CpuGeometry(vaoid);
    // Front face
    cpuCube->data.vertices.emplace_back(-0.5f, -0.5f,  0.5f); // Bottom Left
    cpuCube->data.vertices.emplace_back(0.5f, -0.5f,  0.5f);// Bottom Right
    cpuCube->data.vertices.emplace_back(0.5f,  0.5f,  0.5f); // Top Right
    cpuCube->data.vertices.emplace_back(-0.5f,  0.5f,  0.5f); // Top Left
    //back
    cpuCube->data.vertices.emplace_back(-0.5f, -0.5f,  -0.5f); // Bottom Left
    cpuCube->data.vertices.emplace_back(0.5f, -0.5f,  -0.5f); // Bottom Right
    cpuCube->data.vertices.emplace_back(0.5f,  0.5f,  -0.5f); // Top Right
    cpuCube->data.vertices.emplace_back(-0.5f,  0.5f,  -0.5f); // Top Left
    FaceMaterial cubeFm{};
    for (size_t i = 0; i < indices.size(); i+=3){
        Face f(indices[i], indices[i+1], indices[i+2]);
        cubeFm.faces.push_back(f);
    }
    cpuCube->data.FaceMaterials.push_back(cubeFm);
    std::unordered_map<std::string, Material*> empty;
    GpuObject* cubeObj = VAOFactory::createVAO(cpuCube, empty);
    std::cout << cubeObj->gpuGeometries.size() << std::endl;
    GameObject* object = new GameObject(program, cubeObj, glm::vec3(-10,10,-10),glm::vec3(1, 1, 1),glm::vec3(0, 0, 0));
    GameObject* plane = new GameObject(program, cubeObj, glm::vec3(0,-6,0), glm::vec3(100, .1, 100), glm::vec3(0, 0, 0));
    // end tmp
    Camera* c = new Camera(glm::vec3(0,0,0), FOV, WIDTH/HEIGHT, glm::vec3(0,1,0), 0.0f, 0.0f);

    GpuObject* treeObj = AssetLoader::loadObject("Tree");
    GameObject* tree = new GameObject(lightProgram, treeObj, glm::vec3(10,10,10),glm::vec3(10, 10, 10),glm::vec3(0, 0, 0));

    GpuObject* lptreeObj = AssetLoader::loadObject("low_poly_tree");
    GameObject* lptree = new GameObject(lightProgram, lptreeObj, glm::vec3(-1,1,1),glm::vec3(3, 3, 3),glm::vec3(0, 0, 0));

    GpuObject* catObj = AssetLoader::loadObject("cat");
    GameObject* cat = new GameObject(lightProgram, catObj, glm::vec3(10,10,-10),glm::vec3(.5, .5, .5),glm::vec3(120, 0, 0));

    std::vector<GameObject*> objs = { 
        object,
        plane,
        tree,
        lptree,
        cat,
     };
    Scene s = Scene(c, objs, bp, cubemapTexture);
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



