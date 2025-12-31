#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Material.h"
#include "seri/shader/ShaderLibrary.h"
#include "seri/rendering/RenderingManager.h"

namespace seri
{
	class InfiniteGrid
	{
	public:
		InfiniteGrid()
		{
			Init();
		}

		void Init()
		{
			_material = std::make_shared<Material>();
			_material->shader = ShaderLibrary::Find("grid");;

			SetDefaultPositions();
		}

		void Update()
		{
			auto camera = seri::Graphic::GetCameraPerspective();

			seri::RenderingManager::Begin(camera);

			_material->SetMat4("u_inv_view", glm::inverse(camera->GetView()));
			_material->SetMat4("u_inv_proj", glm::inverse(camera->GetProjection()));
			_material->SetFloat3("u_cam_pos", camera->GetCameraProperties().position);

			seri::RenderCommand renderCommand_grid{};
			renderCommand_grid.name = "infinite_grid";
			renderCommand_grid.desc.depthTestEnabled = true;
			renderCommand_grid.desc.depthWriteEnabled = false;
			renderCommand_grid.desc.blendEnabled = true;
			renderCommand_grid.desc.cullFaceEnabled = false;
			renderCommand_grid.camera = camera;
			renderCommand_grid.drawMode = DrawMode::arrays;
			renderCommand_grid.material = _material;
			renderCommand_grid.vao = _vertexArray;
			renderCommand_grid.rt = seri::RenderingManager::GetEditorRT();
			renderCommand_grid.count = static_cast<uint32_t>(_positions.size());
			seri::RenderingManager::Submit(renderCommand_grid);

			seri::RenderCommand renderCommand_restore{};
			renderCommand_restore.name = "infinite_grid_restore";
			renderCommand_restore.noop = true;
			seri::RenderingManager::Submit(renderCommand_restore);

			seri::RenderingManager::End();
		}

	private:
		void SetDefaultPositions()
		{
			_positions = {
				{ -1.0f, -1.0f, 0.0f },
				{  3.0f, -1.0f, 0.0f },
				{ -1.0f,  3.0f, 0.0f },
			};

			auto vertexBuffer = seri::VertexBufferBase::Create(&_positions[0], _positions.size() * 3 * sizeof(float));
			vertexBuffer->AddElement(
				{ seri::LayoutLocation::vertex, seri::ShaderDataType::float3_type, false }
			);

			_vertexArray = seri::VertexArrayBase::Create();
			_vertexArray->AddVertexBuffer(vertexBuffer);
		}

		std::vector<glm::vec3> _positions;

		std::shared_ptr<Material> _material;
		std::shared_ptr<seri::VertexArrayBase> _vertexArray;

	};
}
