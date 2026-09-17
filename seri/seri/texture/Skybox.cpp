#include "Seripch.h"

#include "seri/texture/Skybox.h"
#include "seri/project/ProjectManager.h"

namespace seri
{
	Skybox::Skybox()
	{
		std::filesystem::path skyboxFolder = seri::project::ProjectManager::GetEngineAssetDirectory() / "textures" / "skybox" / "3";

		for (const char* face : { "px.jpg", "nx.jpg", "py.jpg", "ny.jpg", "pz.jpg", "nz.jpg" })
		{
			_faces.push_back((skyboxFolder / face).string());
		}

		Init();
	}

	Skybox::Skybox(std::vector<std::string> faces) : _faces(std::move(faces))
	{
		Init();
	}

	void Skybox::Init()
	{
		_texture = TextureBase::Create();
		_material = std::make_shared<Material>();
		_material->SetShader(ShaderLibrary::Find("skybox"));
		_material->SetTexture("u_skybox", _texture);

		SetDefaultPositions();
		LoadCubemap();
	}

	void Skybox::Update()
	{
		seri::RenderItem renderItem_skybox{};
		renderItem_skybox.type = PassType::skybox;
		renderItem_skybox.name = "skybox";
		renderItem_skybox.state.depthFunc = DepthFunc::l_equal;
		renderItem_skybox.draw.mode = DrawMode::arrays;
		renderItem_skybox.draw.count = static_cast<uint32_t>(_positions.size());
		renderItem_skybox.material = _material;
		renderItem_skybox.vao = _vertexArray;
		seri::RenderingManager::Submit(std::move(renderItem_skybox));
	}

	void Skybox::SetDefaultPositions()
	{
		_positions = {
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },

			{ -1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, 1.0f, -1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f },

			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },

			{ -1.0f, -1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f },
			{ -1.0f, -1.0f, 1.0f },

			{ -1.0f, 1.0f, -1.0f },
			{ 1.0f, 1.0f, -1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ 1.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, 1.0f },
			{ -1.0f, 1.0f, -1.0f },

			{ -1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ 1.0f, -1.0f, -1.0f },
			{ -1.0f, -1.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f },
		};

		auto vertexBuffer = seri::VertexBufferBase::Create(&_positions[0], _positions.size() * 3 * sizeof(float));
		vertexBuffer->AddElement(
			{ seri::LayoutLocation::vertex, seri::ShaderDataType::float3_type, false }
		);

		_vertexArray = seri::VertexArrayBase::Create();
		_vertexArray->AddVertexBuffer(vertexBuffer);
	}

	void Skybox::LoadCubemap(bool flip)
	{
		if (_faces.size() != 6)
		{
			LIB_LOGGER(error, skybox) << "there should be exactly 6 textures for skybox";
			return;
		}

		TextureDesc desc{};
		desc.flip = flip;
		desc.target = TextureTarget::cube_map;
		desc.wrapS = TextureWrap::clamp_to_edge;
		desc.wrapT = TextureWrap::clamp_to_edge;
		desc.wrapR = TextureWrap::clamp_to_edge;
		desc.magFilter = TextureMagFilter::linear;
		desc.minFilter = TextureMinFilter::linear;
		_texture->Init(desc);
		_texture->LoadCubeMap(_faces);
	}

}
