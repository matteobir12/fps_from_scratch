#ifndef VAO_FACTORY_H
#define VAO_FACTORY_H

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <unordered_map>

#include "CommonStructs.h"

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
    static Rendering::GpuObject* createVAO(Rendering::CpuGeometry* cpuGeo, std::unordered_map<std::string, Rendering::Material*>& materialLibs);
};

#endif