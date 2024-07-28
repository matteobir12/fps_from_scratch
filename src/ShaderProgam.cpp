#include "ShaderProgram.h"
#include <vector>
GLuint ShaderProgram::currentlyActiveShader = 0;

std::string ShaderProgram::readFile(const char* filePath) {
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

GLuint ShaderProgram::loadShader(const char* vertex_path, const char* fragment_path) {
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

    glUseProgram(program);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return program;
}

std::string ShaderProgram::getExecutablePath() {
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


GLuint ShaderProgram::getNoLightShaderProgram() const {
    return programID;
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }

    GLint location = glGetUniformLocation(programID, name.c_str());
    uniformLocationCache[name] = location;
    return location;
}

ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string basePath = getExecutablePath();
    std::string vertexShaderPath = basePath + vertexPath;
    std::string fragmentShaderPath = basePath + fragmentPath;
    if (!std::filesystem::exists(vertexShaderPath)) {
        std::cerr << "Vertex shader file not found at: " << vertexShaderPath << std::endl;
    }

    if (!std::filesystem::exists(fragmentShaderPath)) {
        std::cerr << "Fragment shader file not found at: " << fragmentShaderPath << std::endl;
    }
    programID = loadShader(vertexShaderPath.c_str(),fragmentShaderPath.c_str());
}

void ShaderProgram::use() {
    if (currentlyActiveShader != programID){
        glUseProgram(programID);
        currentlyActiveShader = programID;
    }        
}

GLuint ShaderProgram::getID() const {
    return programID;
}

void ShaderProgram::setUniform(const std::string& name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4* values, int count) {
    glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, glm::value_ptr(values[0]));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value) {
    glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4* values, int count) {
    glUniform4fv(getUniformLocation(name), count, glm::value_ptr(values[0]));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value) {
    glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3* values, int count) {
    glUniform3fv(getUniformLocation(name), count, glm::value_ptr(values[0]));
}

void ShaderProgram::setUniform(const std::string& name, int * value, int count) {
    glUniform1iv(getUniformLocation(name), count, value);
}

void ShaderProgram::setUniform(const std::string& name, bool value) {
    glUniform1i(getUniformLocation(name), value ? GL_TRUE : GL_FALSE);
}

void ShaderProgram::setUniform(const std::string& name, const bool* values, int count) {
    std::vector<GLint> intValues(count);
    for (int i = 0; i < count; ++i) {
        intValues[i] = values[i] ? GL_TRUE : GL_FALSE;
    }
    glUniform1iv(getUniformLocation(name), count, intValues.data());
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(programID);
}
