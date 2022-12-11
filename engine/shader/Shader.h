#pragma once

#include "../logging/Logger.h"

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

    Shader(Shader&& other) noexcept {
        _program = other._program;

        _shouldDeleteThis = false;
        other._shouldDeleteThis = false;
    }

    Shader(Shader& other) noexcept {
        _program = other._program;

        _shouldDeleteThis = false;
        other._shouldDeleteThis = false;
    }

    Shader& operator=(Shader&& other) noexcept {
        _program = other._program;

        _shouldDeleteThis = false;
        other._shouldDeleteThis = false;

        return *this;
    }

    Shader& operator=(Shader& other) noexcept {
        _program = other._program;

        _shouldDeleteThis = false;
        other._shouldDeleteThis = false;

        return *this;
    }

    ~Shader() {
        if (_shouldDeleteThis && _program != 0) {
            disuse();
            del();
        }
    }

    void init(const std::string& vsCodePath, const std::string& fsCodePath, bool readFromFile = true) {
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vsCodePath, readFromFile);
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsCodePath, readFromFile);

        _program = glCreateProgram();
        glAttachShader(_program, vertexShader);
        glAttachShader(_program, fragmentShader);
        glLinkProgram(_program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        checkProgramLinkingError();
    }

    void use() {
        if (!isActiveForUsing()) {
            LOGGER(error, "shader is not active");
            return;
        }

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
    unsigned int compileShader(GLenum type, const std::string& code, bool readFromFile) {
        unsigned int shader = glCreateShader(type);
        std::string codeStr = code;
        if (readFromFile) {
            codeStr = readShaderCode(code);
        }
        const char* codeCStr = codeStr.c_str();
        glShaderSource(shader, 1, &codeCStr, nullptr);
        glCompileShader(shader);
        checkShaderCompilationError(shader);
        return shader;
    }

    std::string readShaderCode(const std::string& shaderCodePath) {
        try {
            std::stringstream ss;
            std::ifstream shaderFile(shaderCodePath);
            if (shaderFile.is_open()) {
                ss << shaderFile.rdbuf();
                shaderFile.close();
            }
            return ss.str();
        }
        catch (const std::exception& ex) {
            LOGGER(error, "exception occurred while reading shader code: " << ex.what());
            return std::string{};
        }
    }

    bool checkShaderCompilationError(const unsigned int shader) {
        int errorStatusSuccess;
        char errorStatusLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &errorStatusSuccess);
        if (!errorStatusSuccess) {
            glGetShaderInfoLog(shader, 512, nullptr, errorStatusLog);
            LOGGER(error, "shader compilation failed: " << std::string{ errorStatusLog });
            return false;
        }

        return true;
    }

    bool checkProgramLinkingError() {
        int errorStatusSuccess;
        char errorStatusLog[512];
        glGetProgramiv(_program, GL_LINK_STATUS, &errorStatusSuccess);
        if (!errorStatusSuccess) {
            glGetShaderInfoLog(_program, 512, nullptr, errorStatusLog);
            LOGGER(error, "program linking failed: " << std::string{ errorStatusLog });
            return false;
        }

        return true;
    }

    bool isActiveForUsing() {
        return _program != 0;
    }

    void del() {
        glDeleteProgram(_program);
        _program = 0;
    }

    unsigned int _program{ 0 };
    bool _shouldDeleteThis{ true };

};
