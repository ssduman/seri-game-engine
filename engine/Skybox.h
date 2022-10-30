#pragma once

#include "Entity.h"
#include "Texture.h"

class Skybox : public Entity {
public:
    Skybox(ICamera* camera) : Entity(camera) {
        LOGGER(info, "skybox init succeeded");
    }

    ~Skybox() override {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);

        LOGGER(info, "skybox delete succeeded");
    }

    void initMVP() override {
        _shader.use();
        _shader.setMat4("u_model", glm::mat4{ 1.0f });
        _shader.setMat4("u_view", glm::mat4(glm::mat3(_camera->getView())));
        _shader.setMat4("u_projection", _camera->getProjection());
        _shader.disuse();
    }

    void setDefaultPositions() {
        const std::vector<glm::vec3> positions = {
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },

            { -1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, -1.0f },
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },

            { 1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },

            { -1.0f, -1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
            { -1.0f, -1.0f, 1.0f },

            { -1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, -1.0f },
            { 1.0f, 1.0f, 1.0f },
            { 1.0f, 1.0f, 1.0f },
            { -1.0f, 1.0f, 1.0f },
            { -1.0f, 1.0f, -1.0f },

            { -1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, -1.0f },
            { 1.0f, -1.0f, -1.0f },
            { -1.0f, -1.0f, 1.0f },
            { 1.0f, -1.0f, 1.0f },
        };

        dataBuffer({ /*size*/ sizeof(positions[0]) * positions.size(), /*data*/ positions.data() });
    }

    void setFaces(const std::vector<std::string>& faces) {
        _faces = faces;
    }

    void loadCubemap(bool flip = false) {
        if (_faces.empty() || _faces.size() != 6) {
            LOGGER(error, "there should be exactly 6 textures for skybox");
            return;
        }

        glGenTextures(1, &_tex);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);

        if (!flip) {
            std::swap(_faces[2], _faces[3]);
        }
        Texture::setTextureFlip(flip);

        int width, height, components;
        for (size_t i = 0; i < _faces.size(); i++) {
            if (auto image = Texture::loadTexture(_faces[i], width, height, components, 0)) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                Texture::unloadTexture(image);
            } else {
                LOGGER(error, "texture " << _faces[i] << " could not loaded");
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void update() override {
        if (_camera) {
            getShader().setMat4("u_view", glm::mat4(glm::mat3(_camera->getView())));
        }
    }

    void render() override {
        _shader.use();
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
        glDrawArrays(_drawMode, 0, _positionsDataCount / 3);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

private:
    unsigned int _tex = 0;
    std::vector<std::string> _faces;

};