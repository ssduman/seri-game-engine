#pragma once

#include "Entity.h"
#include "Texture.h"

class Skybox : public Entity {
public:
    Skybox(ICamera* camera) : Entity(camera) {
        Skybox::init();
        Skybox::initShader();
        loadCubemap();
        setViewProjection();

        LOGGER(info, "skybox init succeeded");
    }

    void setFaces(std::vector<std::string>& faces) {
        _faces = faces;
    }

    void setViewProjection() {
        _shader.use();
        _shader.setMat4("u_view", glm::mat4(glm::mat3(_camera->getView())));
        _shader.setMat4("u_projection", _camera->getProjection());
    }

    void update() override {
        if (_camera->viewUpdated()) {
            setViewProjection();
        }
    }

    void render() override {
        _shader.use();
        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

private:
    void init() override {
        _vertices = {
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
        };

        _faces = {
            "assets/textures/skybox/right.jpg",
            "assets/textures/skybox/left.jpg",
            "assets/textures/skybox/bottom.jpg",
            "assets/textures/skybox/top.jpg",
            "assets/textures/skybox/front.jpg",
            "assets/textures/skybox/back.jpg"
        };

        glGenVertexArrays(1, &_VAO);
        glGenBuffers(1, &_VBO);

        glBindVertexArray(_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);

        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void initShader(const std::string& vsCodePath = "assets/shaders/skybox_vs.shader", const std::string& fsCodePath = "assets/shaders/skybox_fs.shader") override {
        _shader.init(vsCodePath, fsCodePath);
    }

    void loadCubemap(bool flip = false) {
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

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

    unsigned int _texture = 0;
    std::vector<std::string> _faces{};

};