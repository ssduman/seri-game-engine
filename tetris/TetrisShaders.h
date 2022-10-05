#pragma once

#include <string>

static const std::string vertexShader{ R"(
    #version 330 core

    layout(location = 0) in vec2 in_coord;
    layout(location = 2) in vec2 in_texCoord;

    uniform vec2 u_position = vec2(0.0, 0.0);
    uniform mat4 u_projection;

    out vec2 sent_texCoord;

    void main() {
        sent_texCoord = in_texCoord;

        gl_Position = u_projection * vec4(in_coord + u_position, -0.1, 1.0);
    }
)" };

static const std::string fragmentShader{ R"(
    #version 330 core

    in vec2 sent_texCoord;

    uniform vec4 u_color;
    uniform bool u_useColor = false;
    uniform sampler2D u_texture;
    uniform bool u_useTexture = false;

    out vec4 final_color;

    void main() {
        if (u_useTexture && u_useColor) {
            final_color = texture(u_texture, sent_texCoord) * u_color;
        }
        else if (u_useTexture) {
            final_color = texture(u_texture, sent_texCoord);
        }
        else if (u_useColor) {
            final_color = u_color;
        }
        else {
            final_color = vec4(0.1, 0.4, 0.9, 1.0f);
        }
    }
)" };
