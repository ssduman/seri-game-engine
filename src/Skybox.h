#pragma once

#include "Entity.h"

#include <stb_image.h>

class Skybox : public Entity {

public:
    Skybox() : Entity(nullptr) {
        init();
        initShader();
        loadCubemap();

        LOGGER(info, "skybox init succeeded");
    }

    Skybox(Camera* camera) : Entity(camera) {
        init();
        initShader();
        loadCubemap(false);

        LOGGER(info, "skybox init succeeded");
    }

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
            "textures/skybox/right.jpg",
            "textures/skybox/left.jpg",
            "textures/skybox/bottom.jpg",
            "textures/skybox/top.jpg",
            "textures/skybox/front.jpg",
            "textures/skybox/back.jpg"
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

    void initShader(const std::string& vsCodePath = "shaders/skybox_vs.shader", const std::string& fsCodePath = "shaders/skybox_fs.shader") override {
        _shader.init(vsCodePath, fsCodePath);
    }

    void update() override {
        _shader.use();
        _shader.setMat4("u_view", glm::mat4(glm::mat3(_camera->getView())));
        _shader.setMat4("u_projection", _camera->getProjection());
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

    void display(glm::mat4 view, glm::mat4 projection) {
        _shader.use();
        _shader.setMat4("u_view", glm::mat4(glm::mat3(view)));
        _shader.setMat4("u_projection", projection);

        render();
    }

private:
    void loadCubemap(bool flip = true) {
        glGenTextures(1, &_texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

        if (!flip) {
            std::swap(_faces[2], _faces[3]);
        }
        stbi_set_flip_vertically_on_load(flip);
        
        int width, height, components;
        for (auto i = 0; i < _faces.size(); i++) {
            auto data = stbi_load(_faces[i].c_str(), &width, &height, &components, 0);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
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