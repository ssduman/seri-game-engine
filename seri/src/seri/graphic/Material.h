#pragma once

#include "seri/shader/ShaderBase.h"
#include "seri/texture/Texture.h"

namespace seri
{
	class Material
	{
	public:
		Material() = default;

		~Material() = default;

		std::shared_ptr<ShaderBase> shader;

		std::shared_ptr<Texture> texture;

	private:

	};
}
