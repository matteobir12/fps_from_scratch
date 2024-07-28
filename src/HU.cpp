#include "HUD.h"
#include "AssetLoader.h"
#include <iterator>

glm::mat4 HUD::orthoProjection;
std::vector<HUD::twoDObj> HUD::twoDObjs;
ShaderProgram* HUD::shaderProgram = nullptr;
void HUD::Initialize(float aspectRatio, ShaderProgram* noLightProgram) {
    orthoProjection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);

    shaderProgram = noLightProgram;
    SetupBuffers(aspectRatio);
}

void HUD::Render() {
    
    shaderProgram->use();
    shaderProgram->setUniform("uMat", orthoProjection);
    
    shaderProgram->setUniform("uUseTexture", 1);
    // glm::vec4 color(255,0,0,1);
    // shaderProgram->setUniform("u_color", color);
    shaderProgram->setUniform("textureSampler", 0);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST);
    for (const auto& obj : twoDObjs) {
        glBindVertexArray(obj.vao);
        glBindTexture(GL_TEXTURE_2D, obj.texture);
        glDrawElements(GL_TRIANGLES, obj.size, GL_UNSIGNED_INT, 0);
        for (GLenum error = glGetError(); error; error = glGetError()) {
            std::cerr << "OpenGL Error in HUD at draw array (" << error << "): " << std::endl;
        }
    }
    glEnable(GL_DEPTH_TEST);
}


void HUD::SetupBuffers(float aspectRatio) {
    freeAll();
    float ratAsp = 0.1f * aspectRatio;
    float vertices[] = {
        // positions        // texture coords
        -ratAsp,  0.1f,  0.0f, 1.0f, // top left 
        ratAsp,  0.1f,  1.0f, 1.0f, // top right
        ratAsp, -0.1f,  1.0f, 0.0f, // bottom right
        -ratAsp, -0.1f,  0.0f, 0.0f  // bottom left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    twoDObj crosshairObj;
    GLuint tmpBuffers[2];
    glGenVertexArrays(1, &crosshairObj.vao);
    glGenBuffers(2, tmpBuffers);

    glBindVertexArray(crosshairObj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, tmpBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tmpBuffers[1]);
    crosshairObj.size = std::size(indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);

    crosshairObj.texture = AssetLoader::loadTexture("2d/crosshair.png");
    crosshairObj.vbo = tmpBuffers[0];
    crosshairObj.ebo = tmpBuffers[1];

    // Add the crosshair object to the objects vector
    twoDObjs.push_back(crosshairObj);
}

void HUD::freeAll(){
    for (const auto& obj : twoDObjs) {
        glDeleteBuffers(1, &obj.ebo);
        glDeleteBuffers(1, &obj.vbo);
        glDeleteVertexArrays(1, &obj.vao);
        glDeleteTextures(1, &obj.texture);
    }
    twoDObjs.clear();
}

void HUD::windowResize(float aspectRatio) {
    orthoProjection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    SetupBuffers(aspectRatio);
}