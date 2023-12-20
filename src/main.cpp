#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include "ShaderProgram.h"
#include "Camera.h"
#include "GameObject.h"
#include "VAOFactory.h"

class Scene {
    private:
        Camera* camera;
        std::vector<GameObject*> objects;

    public:
        Scene(Camera* camera, const std::vector<GameObject*>& objects) : camera(camera), objects(objects) {}

        void update() {
            GLenum err;
            while((err = glGetError()) != GL_NO_ERROR) {
                std::cerr << "OpenGL error: " << err << std::endl;
            }
        }

        void render() {
            for (GameObject*& object : objects) {
               
                object->draw(camera->GetViewProjectionMatrix());
            }
        }

        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
            Scene* myObject = static_cast<Scene*>(glfwGetWindowUserPointer(window));
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
            if (key == GLFW_KEY_W){
                myObject->camera->Move(glm::vec3(10,0,0));
                std::cout << "mobe " << std::endl;
            }
            if (key == GLFW_KEY_S){
                myObject->camera->Move(glm::vec3(-10,0,0));
            }
            if (key == GLFW_KEY_D){
                myObject->camera->Move(glm::vec3(0,10,0));
            }
            if (key == GLFW_KEY_A){
                myObject->camera->Move(glm::vec3(0,-10,0));
            }


        }

        double lastX = 400, lastY = 300; // Assuming 800x600 window size update for dynamic
        bool firstMouse = true;

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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
            std::cout << xpos << " " << std::endl;

            myObject->camera->Rotate(xoffset, yoffset);
        }
};

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


int main(int argc, char** argv) {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "My OpenGL Window", NULL, NULL);
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
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    std::vector<GLuint> glbuffs = VAOFactory::createVAO(vertices, &indices);
    std::string vertexShaderPath = "../shaders/no_lighting.vert";
    std::string fragmentShaderPath = "../shaders/no_lighting.frag";
    ShaderProgram* program = new ShaderProgram(vertexShaderPath, fragmentShaderPath);
    GameObject* object = new GameObject(program, glbuffs, 80, glm::vec3(-100,0,-100),glm::vec3(1, 1, 1),glm::vec3(27, 0, 0));
    Camera* c = new Camera(glm::vec3(0,-2,0),glm::vec3(-1,-1,-1),160, 800.0f/600.0f);
    std::vector<GameObject*> objs = { 
        object,
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,-10,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(0,-10,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,0,-10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(-10,-10,0)),
        // new GameObject(program, glbuffs, 10, glm::vec3(10,10,10)),
        // new GameObject(program, glbuffs, 10, glm::vec3(0,-10,0)),
     };
    Scene s = Scene(c,objs);
    glfwSetWindowUserPointer(window, &s);
    glfwSetCursorPosCallback(window, Scene::mouse_callback);
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