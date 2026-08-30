#include "Seripch.h"

#include "seri/texture/Skybox.h"

namespace seri
{
	Skybox::Skybox()
	{
		_faces = {
			"assets/textures/skybox/1/right.jpg",
			"assets/textures/skybox/1/left.jpg",
			"assets/textures/skybox/1/top.jpg",
			"assets/textures/skybox/1/bottom.jpg",
			"assets/textures/skybox/1/front.jpg",
			"assets/textures/skybox/1/back.jpg",
		};

		_faces = {
			"assets/textures/skybox/2/px.jpg",
			"assets/textures/skybox/2/nx.jpg",
			"assets/textures/skybox/2/py.jpg",
			"assets/textures/skybox/2/ny.jpg",
			"assets/textures/skybox/2/pz.jpg",
			"assets/textures/skybox/2/nz.jpg",
		};

		_faces = {
			"assets/textures/skybox/3/px.jpg",
			"assets/textures/skybox/3/nx.jpg",
			"assets/textures/skybox/3/py.jpg",
			"assets/textures/skybox/3/ny.jpg",
			"assets/textures/skybox/3/pz.jpg",
			"assets/textures/skybox/3/nz.jpg",
		};

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
		_material->SetMat4(literals::kUniformViewSkybox, glm::mat4(glm::mat3(seri::Graphic::GetCameraPerspective()->GetView())));

		seri::RenderCommand renderCommand_skybox{};
		renderCommand_skybox.name = "skybox";
		renderCommand_skybox.state.depthFunc = DepthFunc::l_equal;
		renderCommand_skybox.camera = seri::Graphic::GetCameraPerspective();
		renderCommand_skybox.draw.mode = DrawMode::arrays;
		renderCommand_skybox.draw.count = static_cast<uint32_t>(_positions.size());
		renderCommand_skybox.material = _material;
		renderCommand_skybox.vao = _vertexArray;
		renderCommand_skybox.rt = seri::RenderingManager::GetEditorRT();
		seri::RenderingManager::Submit(renderCommand_skybox);

		seri::RenderCommand renderCommand_restore{};
		renderCommand_restore.name = "skybox_restore";
		renderCommand_restore.noop = true;
		seri::RenderingManager::Submit(renderCommand_restore);
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
			LOGGER(error, "[skybox] there should be exactly 6 textures for skybox");
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
