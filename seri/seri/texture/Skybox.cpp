#include "Seripch.h"

#include "seri/texture/Skybox.h"

namespace seri
{
	Skybox::Skybox()
	{
		Init();
	}

	Skybox::Skybox(std::vector<std::string> faces) : _faces(std::move(faces))
	{
		Init();
	}

	void Skybox::Init()
	{
		type = seri::asset::AssetType::skybox;

		_material = std::make_shared<Material>();
		_material->SetShader(ShaderLibrary::Find("skybox"));

		SetDefaultPositions();
		LoadCubemap();
	}

	void Skybox::SetFaces(std::vector<std::string> faces)
	{
		_faces = std::move(faces);

		LoadCubemap();
	}

	void Skybox::Update()
	{
		if (!_texture)
		{
			return;
		}

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
		_texture = nullptr;

		if (_faces.empty())
		{
			return;
		}

		if (_faces.size() != 6)
		{
			LIB_LOGGER(error, skybox) << "there should be exactly 6 textures for skybox";
			return;
		}

		_texture = TextureBase::Create();
		_material->SetTexture("u_skybox", _texture);

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
