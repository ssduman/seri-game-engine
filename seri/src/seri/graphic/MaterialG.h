#pragma once

#include "seri/graphic/MeshG.h"
#include "seri/shader/Shader.h"
#include "seri/texture/Texture.h"

class MaterialG
{
public:
	MaterialG() = default;

	~MaterialG() = default;

	std::shared_ptr<Shader> shader;

	std::shared_ptr<Texture> texture;

private:

};
