#ifndef VAO_FACTORY_H
#define VAO_FACTORY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

class VAOFactory {
public:
    struct Attribute {
        GLuint index = 0;
        GLint size = 3;
        GLenum type = GL_FLOAT;
        GLsizei stride = 0;
        size_t offset = 0;
    
        Attribute() = default;
        Attribute(GLuint idx, GLint sz, GLenum tp, GLsizei str, size_t off);
    };

    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const std::vector<unsigned int>* indices = nullptr);
    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const Attribute& attributes, const std::vector<unsigned int>* indices = nullptr);
    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<unsigned int>* indices = nullptr);
};

#endif