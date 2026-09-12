#pragma once

#include <glm/glm.hpp>

namespace seri
{
	class Mesh;
}

namespace seri::system
{
	class SpriteRendererSystem
	{
	public:
		static void Update();

	private:
		static void BuildQuad(Mesh& mesh, const glm::vec2& size, const glm::vec2& pivot, bool flipX, bool flipY);
	};
}
