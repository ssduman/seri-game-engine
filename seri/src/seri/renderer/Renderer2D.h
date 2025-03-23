#pragma once

#include "seri/shader/Shader.h"
#include "seri/shader/ShaderManager.h"
#include <seri/texture/Texture.h>
#include <seri/math/Transform.h>
#include "OpenGLEngineBackend.h"

struct RenderData {
    unsigned int vertexId{ 0 };
    unsigned int bufferId{ 0 };
    unsigned int elementsId{ 0 };
    unsigned int shaderId{ 0 };
    unsigned int textureId{ 0 };
    unsigned int drawCount{ 0 };
    unsigned int drawType{ 0 };
};

class Renderer {
public:
    Renderer() = default;

    Renderer(Renderer&&) = delete;

    Renderer(const Renderer&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    ~Renderer() = default;

    static void init() {}

    static void drawLine() {}

private:
    //static Shader _shader;
    //static ShaderManager _shaderManager;
    //static Texture _texture;
    //static Transform _transform;
    //static OpenGLEngineBackend _engineBackend;

};
