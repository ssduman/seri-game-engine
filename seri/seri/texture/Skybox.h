#pragma once

#include "seri/util/Util.h"
#include "seri/asset/AssetBase.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/texture/TextureBase.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/render/RenderingManager.h"

#include <array>

namespace seri
{
	class Skybox : public seri::asset::AssetBase
	{
	public:
		Skybox();
		Skybox(std::vector<std::string> faces);

		void Init();
		void Update();

		void SetFaces(std::vector<std::string> faces);

		std::array<uint64_t, 6> faceIds{};

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
