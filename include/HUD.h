#ifndef HUD_H
#define HUD_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include "ShaderProgram.h"
#include <vector>

class HUD {

public:

    static ShaderProgram* shaderProgram;

    static void Initialize(float aspectRatio, ShaderProgram* noLightProgram);
    static void Render();

private:
    struct twoDObj {
        GLuint vao, vbo, ebo;
        GLuint texture;
        int size;
        twoDObj() = default;
        twoDObj(GLuint vao, GLuint vbo, GLuint ebo, GLuint texture, int size) 
        : vao(vao), vbo(vbo), ebo(ebo), texture(texture), size(size)
        {}
    };
    static glm::mat4 orthoProjection;
    
    static std::vector<twoDObj> twoDObjs;

    static void SetupBuffers(float aspectRatio);
    static void windowResize(float aspectRatio);
    static void freeAll();
};

#endif
