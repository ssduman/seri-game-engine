#include "Seripch.h"

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/camera/EditorCamera.h"
#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Model.h"
#include "seri/graphic/Material.h"
#include "seri/rendering/render/RenderingManager.h"
#include "seri/scene/SceneManager.h"

namespace seri
{
	void Graphic::SetCameraUI(std::shared_ptr<CameraBase> camera)
	{
		GetInstance()._cameraUI = camera;
	}

	void Graphic::SetEditorCamera(std::shared_ptr<EditorCamera> camera)
	{
		GetInstance()._cameraEditor = camera;
	}

	void Graphic::SetRuntimeCamera(std::shared_ptr<CameraBase> camera)
	{
		GetInstance()._cameraRuntime = camera;
	}

	std::shared_ptr<CameraBase> Graphic::GetCameraUI()
	{
		return GetInstance()._cameraUI;
	}

	std::shared_ptr<EditorCamera> Graphic::GetEditorCamera()
	{
		return GetInstance()._cameraEditor;
	}

	std::shared_ptr<CameraBase> Graphic::GetRuntimeCamera()
	{
		return GetInstance()._cameraRuntime;
	}

	std::shared_ptr<CameraBase> Graphic::GetActiveCamera()
	{
		return GetInstance()._cameraEditor;
	}

	void Graphic::DrawModel(const std::shared_ptr<Model>& model, const std::shared_ptr<Material>& material, const glm::mat4& trs)
	{
		for (const auto& mesh : model->meshes)
		{
			Graphic::Draw(mesh, material, trs);
		}
	}

	void Graphic::Draw(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& trs, PassType passType)
	{
		material->SetFloat4(literals::kUniformColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		RenderItem cmd{};
		cmd.type = passType;
		cmd.name = "draw";
		cmd.material = material;
		cmd.model = trs * mesh->transformation;
		cmd.vao = mesh->GetVao();

		if (mesh->bonesForVertices.size() > 0)
		{
			mesh->FillBonePalette(cmd.bones);
		}

		RenderingManager::Submit(std::move(cmd));
	}

	void Graphic::DrawInstanced(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& trs)
	{
		if (trs.empty())
		{
			return;
		}

		mesh->UploadInstanced(trs);

		material->SetFloat4(literals::kUniformColor, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });

		RenderItem cmd{};
		cmd.type = PassType::opaque;
		cmd.name = "draw_instanced";
		cmd.material = material;
		cmd.vao = mesh->GetVao();
		cmd.draw.mode = DrawMode::elements_instanced;
		cmd.draw.instanceCount = static_cast<uint32_t>(trs.size());
		RenderingManager::Submit(cmd);
	}

}
