#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>
#include <string>
#include <unordered_map>

class ShaderProgram {
private:

    GLuint programID;
    std::unordered_map<std::string, GLint> uniformLocationCache;

    std::string readFile(const char* filePath);

    GLuint loadShader(const char* vertex_path, const char* fragment_path);

    std::string getExecutablePath();

    GLuint getNoLightShaderProgram();

    GLint getUniformLocation(const std::string& name);

public:
    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    ~ShaderProgram();

    void use();

    GLuint getID() const;

    void setUniform(const std::string& name, float value);

};
#endif
