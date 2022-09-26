#pragma once

#include "Logger.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

template <typename T>
class Vec3;

template <typename T>
class Vec4;

template <typename T>
class Mat4;

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
        glShaderSource(_vertexShader, 1, &vsSourceCode, nullptr);
        glCompileShader(_vertexShader);

        _fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        std::string fsSourceCodeStr = readShaderCode(fsCodePath);
        const char* fsSourceCode = fsSourceCodeStr.c_str();
        glShaderSource(_fragmentShader, 1, &fsSourceCode, nullptr);
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

    void setBool(const std::string& name, bool value) const {
        glUniform1i(getUniformLocation(name), value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(getUniformLocation(name), value);
    }

    void setFloat(const std::string& name, float value) const {
        glUniform1f(getUniformLocation(name), value);
    }

    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(getUniformLocation(name), x, y);
    }

    void setVec2(const std::string& name, glm::vec2 value) const {
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    template <typename T = float>
    void setVec3(const std::string& name, Vec3<T>& value) const {
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    template <typename T = float>
    void setVec4(const std::string& name, Vec4<T>& value) const {
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
    }

    template <typename T = float>
    void setMat4(const std::string& name, Mat4<T>& mat) const {
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

    void checkShaderCompilationError() {
        checkShaderCompilationError(_vertexShader);
        checkShaderCompilationError(_fragmentShader);
    }

    void checkShaderCompilationError(const unsigned int _s) {
        glGetShaderiv(_s, GL_COMPILE_STATUS, &_errorStatusSuccess);
        if (!_errorStatusSuccess) {
            glGetShaderInfoLog(_s, 512, nullptr, _errorStatusLog);
            LOGGER(error, "error: shader compilation failed: " << _errorStatusLog);
        }
    }

    void checkProgramError() {
        glGetProgramiv(_program, GL_LINK_STATUS, &_errorStatusSuccess);
        if (!_errorStatusSuccess) {
            glGetShaderInfoLog(_program, 512, nullptr, _errorStatusLog);
            LOGGER(error, "error: program linking failed: " << _errorStatusLog);
        }
    }

    int _errorStatusSuccess = -1;
    char _errorStatusLog[512] = {};
    unsigned int _program = 0;
    unsigned int _vertexShader = 0;
    unsigned int _fragmentShader = 0;

};