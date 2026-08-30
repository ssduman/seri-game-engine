#include "Seripch.h"

#include "seri/texture/InfiniteGrid.h"

namespace seri
{
	InfiniteGrid::InfiniteGrid()
	{
		Init();
	}

	void InfiniteGrid::Init()
	{
		_material = std::make_shared<Material>();
		_material->SetShader(ShaderLibrary::Find("grid"));

		SetDefaultPositions();
	}

	void InfiniteGrid::Update()
	{
		auto camera = seri::Graphic::GetCameraPerspective();

		_material->SetMat4("u_inv_view", glm::inverse(camera->GetView()));
		_material->SetMat4("u_inv_proj", glm::inverse(camera->GetProjection()));
		_material->SetFloat3("u_cam_pos", camera->GetCameraProperties().position);

		seri::RenderCommand renderCommand_grid{};
		renderCommand_grid.name = "infinite_grid";
		renderCommand_grid.state.depthTestEnabled = true;
		renderCommand_grid.state.depthWriteEnabled = false;
		renderCommand_grid.state.blendEnabled = true;
		renderCommand_grid.state.cullFaceEnabled = false;
		renderCommand_grid.camera = camera;
		renderCommand_grid.draw.mode = DrawMode::arrays;
		renderCommand_grid.draw.count = static_cast<uint32_t>(_positions.size());
		renderCommand_grid.material = _material;
		renderCommand_grid.vao = _vertexArray;
		renderCommand_grid.rt = seri::RenderingManager::GetEditorRT();
		seri::RenderingManager::Submit(renderCommand_grid);

		seri::RenderCommand renderCommand_restore{};
		renderCommand_restore.name = "infinite_grid_restore";
		renderCommand_restore.noop = true;
		seri::RenderingManager::Submit(renderCommand_restore);
	}

	void InfiniteGrid::SetDefaultPositions()
	{
		_positions = {
			{ -1.0f, -1.0f, 0.0f },
			{ +3.0f, -1.0f, 0.0f },
			{ -1.0f, +3.0f, 0.0f },
		};

		auto vertexBuffer = seri::VertexBufferBase::Create(&_positions[0], _positions.size() * 3 * sizeof(float));
		vertexBuffer->AddElement(
			{ seri::LayoutLocation::vertex, seri::ShaderDataType::float3_type, false }
		);

		_vertexArray = seri::VertexArrayBase::Create();
		_vertexArray->AddVertexBuffer(vertexBuffer);
	}
}
