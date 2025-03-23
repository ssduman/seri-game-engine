#pragma once

#include "seri/shader/Shader.h"
#include "seri/camera/ICamera.h"

#include <memory>

class ShaderManager {
public:
    ShaderManager() = delete;

    ShaderManager(Shader& shader) : _shader(shader) {}

    ShaderManager(ShaderManager& other) = default;

    ShaderManager(ShaderManager&& other) = default;

    ShaderManager& operator=(ShaderManager& other) = default;

    ShaderManager& operator=(ShaderManager&& other) = default;

    ~ShaderManager() = default;

    void setTRS(Shader& shader, const std::shared_ptr<ICamera>& camera) {
        ShaderManager::Use(shader);
        ShaderManager::SetMat4(shader, "u_model", glm::mat4{ 1.0f });
        ShaderManager::SetMat4(shader, "u_view", camera->getView());
        ShaderManager::SetMat4(shader, "u_projection", camera->getProjection());
        ShaderManager::Disuse();
    }

    void initMVP(const std::shared_ptr<ICamera>& camera) {
        ShaderManager::Use(_shader);
        ShaderManager::SetMat4(_shader, "u_model", glm::mat4{ 1.0f });
        ShaderManager::SetMat4(_shader, "u_view", camera->getView());
        ShaderManager::SetMat4(_shader, "u_projection", camera->getProjection());
        ShaderManager::Disuse();
    }

    void initVP(const std::shared_ptr<ICamera>& camera) {
        ShaderManager::Use(_shader);
        ShaderManager::SetMat4(_shader, "u_view", camera->getView());
        ShaderManager::SetMat4(_shader, "u_projection", camera->getProjection());
        ShaderManager::Disuse();
    }

    void setModel(const glm::mat4& model) {
        ShaderManager::Use(_shader);
        ShaderManager::SetMat4(_shader, "u_model", model);
        ShaderManager::Disuse();
    }

    void setView(const glm::mat4& view) {
        ShaderManager::Use(_shader);
        ShaderManager::SetMat4(_shader, "u_view", view);
        ShaderManager::Disuse();
    }

    void setProjection(const glm::mat4& projection) {
        ShaderManager::Use(_shader);
        ShaderManager::SetMat4(_shader, "u_projection", projection);
        ShaderManager::Disuse();
    }

    void setPosition(const glm::vec2& position) {
        ShaderManager::Use(_shader);
        ShaderManager::SetVec2(_shader, "u_position", position);
        ShaderManager::Disuse();
    }

    void setPosition(const glm::vec3& position) {
        ShaderManager::Use(_shader);
        ShaderManager::SetVec3(_shader, "u_position", position);
        ShaderManager::Disuse();
    }

    void setColor(const glm::vec4& color) {
        ShaderManager::Use(_shader);
        ShaderManager::SetVec4(_shader, "u_color", color);
        ShaderManager::Disuse();
    }

    void useColors(bool flag) {
        ShaderManager::Use(_shader);
        ShaderManager::SetBool(_shader, "u_useColors", flag);
        ShaderManager::Disuse();
    }

    void useTexture(bool flag) {
        ShaderManager::Use(_shader);
        ShaderManager::SetBool(_shader, "u_useTexture", flag);
        ShaderManager::Disuse();
    }

    static Shader Create() {
        return Shader{};
    }

    static void Init(Shader& shader, const char* vsCodePath, const char* fsCodePath, bool readFromFile = true) {
        shader.init(vsCodePath, fsCodePath, readFromFile);
    }

    static void Use(Shader& shader) {
        shader.use();
    }

    static void Use(const std::shared_ptr<Shader>& shader) {
        shader->use();
    }

    static void Disuse() {
        glUseProgram(0);
    }

    static void Disuse(Shader& shader) {
        shader.disuse();
    }

    static int GetUniformLocation(const Shader& shader, const char* name) {
        return glGetUniformLocation(shader.getProgram(), name);
    }

    static int GetUniformLocation(const std::shared_ptr<Shader>& shader, const char* name) {
        return glGetUniformLocation(shader->getProgram(), name);
    }

    static void SetBool(const Shader& shader, const char* name, bool val) {
        glUniform1i(ShaderManager::GetUniformLocation(shader, name), val);
    }

    static void SetInt(const Shader& shader, const char* name, int val) {
        glUniform1i(ShaderManager::GetUniformLocation(shader, name), val);
    }

    static void SetFloat(const Shader& shader, const char* name, float val) {
        glUniform1f(ShaderManager::GetUniformLocation(shader, name), val);
    }

    static void SetVec2(const Shader& shader, const char* name, glm::vec2 val) {
        glUniform2fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
    }

    static void SetVec3(const Shader& shader, const char* name, const glm::vec3& val) {
        glUniform3fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
    }

    static void SetVec4(const Shader& shader, const char* name, const glm::vec4& val) {
        glUniform4fv(ShaderManager::GetUniformLocation(shader, name), 1, &val[0]);
    }

    static void SetMat4(const Shader& shader, const char* name, const glm::mat4& val) {
        glUniformMatrix4fv(ShaderManager::GetUniformLocation(shader, name), 1, GL_FALSE, &val[0][0]);
    }

    static void SetMat4(const std::shared_ptr<Shader>& shader, const char* name, const glm::mat4& val) {
        glUniformMatrix4fv(ShaderManager::GetUniformLocation(shader, name), 1, GL_FALSE, &val[0][0]);
    }
private:
    Shader& _shader;

};