#pragma once

#include <memory>

namespace seri
{
	class Material;
}

namespace seri::system
{
	class MeshRendererSystem
	{
	public:
		static void Update();

	private:
		static std::shared_ptr<Material> GetShadowMaterial(const std::shared_ptr<Material>& material);
	};
}
