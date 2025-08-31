#pragma once

#include "seri/shader/ShaderBase.h"
#include "seri/texture/TextureBase.h"

namespace seri
{
	class Material
	{
	public:
		Material() = default;

		~Material() = default;

		std::shared_ptr<ShaderBase> shader;

		std::shared_ptr<TextureBase> texture;

	private:

	};
}
