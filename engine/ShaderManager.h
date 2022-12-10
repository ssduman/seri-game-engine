#pragma once

#include "ICamera.h"
#include "Shader.h"

class ShaderManager {
public:
    ShaderManager() = delete;

    ShaderManager(Shader& shader) : _shader(shader) {}

    ShaderManager(ShaderManager&& other) noexcept : _shader(other._shader) {}

    ShaderManager(const ShaderManager& other) noexcept : _shader(other._shader) {}

    ShaderManager& operator=(ShaderManager&& other) noexcept {
        _shader = other._shader;

        return *this;
    }

    ShaderManager& operator=(const ShaderManager& other) noexcept {
        _shader = other._shader;

        return *this;
    }

    ~ShaderManager() = default;

    void initMVP(std::shared_ptr<ICamera> camera) {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", camera->getView());
        _shader.setMat4("u_projection", camera->getProjection());
        _shader.disuse();
    }

    void setModel(const glm::mat4& model) {
        _shader.use();
        _shader.setMat4("u_model", model);
        _shader.disuse();
    }

    void setView(const glm::mat4& view) {
        _shader.use();
        _shader.setMat4("u_view", view);
        _shader.disuse();
    }

    void setProjection(const glm::mat4& projection) {
        _shader.use();
        _shader.setMat4("u_projection", projection);
        _shader.disuse();
    }

    void setPosition(const glm::vec2& position) {
        _shader.use();
        _shader.setVec2("u_position", position);
        _shader.disuse();
    }

    void setPosition(const glm::vec3& position) {
        _shader.use();
        _shader.setVec3("u_position", position);
        _shader.disuse();
    }

    void setColor(const glm::vec4& color) {
        useColor(true);
        _shader.use();
        _shader.setVec4("u_color", color);
        _shader.disuse();
    }

    void useColor(bool flag) {
        _shader.use();
        _shader.setBool("u_useColor", true);
        _shader.disuse();
    }

    void useColors(bool flag) {
        _shader.use();
        _shader.setBool("u_useColors", flag);
        _shader.disuse();
    }

    void useTexture(bool flag) {
        _shader.use();
        _shader.setBool("u_useTexture", flag);
        _shader.disuse();
    }

private:
    Shader& _shader;

};