#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/render/RenderingManager.h"

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
