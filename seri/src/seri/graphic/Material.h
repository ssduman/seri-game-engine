#pragma once

#include "seri/shader/Shader.h"
#include "seri/texture/Texture.h"

namespace seri
{
	class Material
	{
	public:
		Material() = default;

		~Material() = default;

		std::shared_ptr<Shader> shader;

		std::shared_ptr<Texture> texture;

	private:

	};
}
