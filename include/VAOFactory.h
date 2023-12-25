#ifndef VAO_FACTORY_H
#define VAO_FACTORY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>

class VAOFactory {
private:
    std::unordered_map<std::string, GLint> uniformLocationCache;
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

    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const char * vaoId, const std::vector<unsigned int>* indices = nullptr);
    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const char * vaoId, const Attribute& attributes, const std::vector<unsigned int>* indices = nullptr);
    static std::vector<GLuint> createVAO(const std::vector<float>& vertices, const char * vaoId, const std::vector<float>& normals, const std::vector<unsigned int>* indices = nullptr);
};

#endif