#pragma once

#include "util/Util.h"
#include "camera/CameraBase.h"
#include "graphic/Graphic.h"
#include "graphic/Material.h"
#include "shader/ShaderLibrary.h"
#include "rendering/render/RenderingManager.h"

namespace seri
{
	class InfiniteGrid
	{
	public:
		InfiniteGrid();

		void Init();

		void Update();

	private:
		void SetDefaultPositions();

		std::vector<glm::vec3> _positions;

		std::shared_ptr<Material> _material;
		std::shared_ptr<seri::VertexArrayBase> _vertexArray;

	};
}
