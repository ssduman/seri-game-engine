#pragma once

#include "util/Util.h"
#include "camera/CameraBase.h"
#include "graphic/Graphic.h"
#include "graphic/Material.h"
#include "texture/TextureBase.h"
#include "shader/ShaderLibrary.h"
#include "rendering/render/RenderingManager.h"

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
