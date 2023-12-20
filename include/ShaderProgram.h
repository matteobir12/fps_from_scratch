#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <glm/ext.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <unordered_map>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif

class ShaderProgram {
private:

    GLuint programID;
    std::unordered_map<std::string, GLint> uniformLocationCache;

    std::string readFile(const char* filePath);

    GLuint loadShader(const char* vertex_path, const char* fragment_path);

    std::string getExecutablePath();

    GLuint getNoLightShaderProgram() const;

    GLint getUniformLocation(const std::string& name);

public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);
    ~ShaderProgram();

    void use();

    GLuint getID() const;

    void setUniform(const std::string& name, float value);
    void setUniform(const std::string& name, glm::mat4& value);
    void setUniform(const std::string& name, glm::vec4& value);
};
#endif
