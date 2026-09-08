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
		auto camera = seri::Graphic::GetActiveCamera();

		_material->SetMat4("u_inv_view", glm::inverse(camera->GetView()));
		_material->SetMat4("u_inv_proj", glm::inverse(camera->GetProjection()));
		_material->SetFloat3("u_cam_pos", camera->GetCameraProperties().position);

		seri::RenderItem renderItem_grid{};
		renderItem_grid.type = PassType::debug;
		renderItem_grid.name = "infinite_grid";
		renderItem_grid.state.depthTestEnabled = true;
		renderItem_grid.state.depthWriteEnabled = false;
		renderItem_grid.state.blendEnabled = true;
		renderItem_grid.state.cullFaceEnabled = false;
		renderItem_grid.draw.mode = DrawMode::arrays;
		renderItem_grid.draw.count = static_cast<uint32_t>(_positions.size());
		renderItem_grid.material = _material;
		renderItem_grid.vao = _vertexArray;
		seri::RenderingManager::Submit(std::move(renderItem_grid));
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
