#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/texture/TextureBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/RenderingManager.h"

namespace seri
{
	class Skybox
	{
	public:
		Skybox();
		Skybox(std::vector<std::string> faces);

		void Init();
		void Update();

	private:
		void SetDefaultPositions();
		void LoadCubemap(bool flip = false);

		std::vector<std::string> _faces;
		std::vector<glm::vec3> _positions;

		std::shared_ptr<Material> _material;
		std::shared_ptr<TextureBase> _texture;
		std::shared_ptr<seri::VertexArrayBase> _vertexArray;

	};
}
