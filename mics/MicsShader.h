#pragma once

#include <string>

static const std::string vertexShader{ R"(
    #version 330 core

    layout(location = 0) in vec3 in_coord;
    layout(location = 1) in vec4 in_color;
    layout(location = 2) in vec2 in_texCoord;
    layout(location = 3) in vec2 in_normal;

    uniform vec3 u_position = vec3(0.0, 0.0, 0.0);
    uniform mat4 u_model;
    uniform mat4 u_view;
    uniform mat4 u_projection;

    out vec4 sent_color;
    out vec2 sent_texCoord;

    void main() {
        sent_color = in_color;
        sent_texCoord = in_texCoord;

        gl_Position = u_projection * u_view * u_model * vec4(in_coord + u_position, 1.0);
    }
)" };

static const std::string fragmentShader{ R"(
    #version 330 core

    in vec4 sent_color;
    in vec2 sent_texCoord;

    uniform sampler2D u_texture;
    uniform bool u_useColors = false;
    uniform bool u_useTexture = false;
    uniform vec4 u_color = vec4(0.1, 0.4, 0.9, 1.0f);

    out vec4 final_color;

    void main() {
        final_color = u_color;

        if (u_useColors) {
            final_color = sent_color;
            return;
        }
        if (u_useTexture) {
            final_color = texture(u_texture, sent_texCoord);
            return;
        }
    }
)" };
