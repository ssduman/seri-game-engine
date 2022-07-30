#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    Shader() = default;

    ~Shader() {
        disuse();
        glDeleteProgram(_program);
    }

    void init(const std::string& vsCodePath, const std::string& fsCodePath) {
        _vertexShader = glCreateShader(GL_VERTEX_SHADER);
        std::string vsSourceCodeStr = readShaderCode(vsCodePath);
        const char* vsSourceCode = vsSourceCodeStr.c_str();
        glShaderSource(_vertexShader, 1, &vsSourceCode, NULL);
        glCompileShader(_vertexShader);

        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fsSourceCodeStr = readShaderCode(fsCodePath);
        const char* fsSourceCode = fsSourceCodeStr.c_str();
        glShaderSource(_fragmentShader, 1, &fsSourceCode, NULL);
        glCompileShader(_fragmentShader);

        checkShaderCompilationError();

        _program = glCreateProgram();
        glAttachShader(_program, _vertexShader);
        glAttachShader(_program, _fragmentShader);
        glLinkProgram(_program);

        checkProgramError();

        glDeleteShader(_vertexShader);
        glDeleteShader(_fragmentShader);
    }

    void use() {
        glUseProgram(_program);
    }

    void disuse() {
        glUseProgram(0);
    }

    int getUniformLocation(const std::string& name) const {
        return glGetUniformLocation(_program, name.c_str());
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(getUniformLocation(name), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

private:
    std::string readShaderCode(const std::string& shaderCodePath) {
        std::stringstream ss;
        std::ifstream shaderFile(shaderCodePath);
        if (shaderFile.is_open()) {
            ss << shaderFile.rdbuf();
            shaderFile.close();
        }
        return ss.str();
    }

    inline void checkShaderCompilationError() {
        checkShaderCompilationError(_vertexShader);
        checkShaderCompilationError(_fragmentShader);
    }

    inline void checkShaderCompilationError(const unsigned int _s) {
        glGetShaderiv(_s, GL_COMPILE_STATUS, &_errorStatusSuccess);
        if (!_errorStatusSuccess) {
            glGetShaderInfoLog(_s, 512, NULL, _errorStatusLog);
            std::cerr << "error: shader compilation failed: " << _errorStatusLog << std::endl;
        }
    }

    inline void checkProgramError() {
        glGetProgramiv(_program, GL_LINK_STATUS, &_errorStatusSuccess);
        if (!_errorStatusSuccess) {
            glGetShaderInfoLog(_program, 512, NULL, _errorStatusLog);
            std::cerr << "error: program linking failed: " << _errorStatusLog << std::endl;
        }
    }

    int _errorStatusSuccess;
    char _errorStatusLog[512];
    unsigned int _program = 0;
    unsigned int _vertexShader = 0;
    unsigned int _fragmentShader = 0;

};
