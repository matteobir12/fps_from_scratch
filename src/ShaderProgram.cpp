#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif


std::string readFile(const char* filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

GLuint loadShader(const char* vertex_path, const char* fragment_path) {
    std::string vertexCode = readFile(vertex_path);
    std::string fragmentCode = readFile(fragment_path);

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

std::string getExecutablePath() {
    char buffer[1024];
    #ifdef _WIN32
    GetModuleFileNameA(NULL, buffer, sizeof(buffer));
    #elif __linux__
    ssize_t len = ::readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    }
    #endif
    std::string executablePath(buffer);
    size_t pos = executablePath.find_last_of("\\/");
    if (pos != std::string::npos) {
        executablePath = executablePath.substr(0, pos + 1);
    }
    return executablePath;
}


GLuint getShaderProgram() {
    static GLuint shaderProgram;
    if (!shaderProgram) {
        std::string basePath = getExecutablePath();
        std::string vertexShaderPath = basePath + "../shaders/vertex_shader.glsl";
        std::string fragmentShaderPath = basePath + "../shaders/fragment_shader.glsl";
        shaderProgram = loadShader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
    }
    return shaderProgram;
}
