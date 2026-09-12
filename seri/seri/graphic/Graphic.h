#pragma once

#include "seri/core/Singleton.h"

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/render/PipelineBase.h"
#include "seri/rendering/common/FramebufferBase.h"

namespace seri
{
	class Mesh;
	class Model;
	class Material;
	class EditorCamera;

	class Graphic : public seri::Singleton<Graphic>
	{
	public:
		static void Init()
		{
			GetInstance();
		}

		static void SetCameraUI(std::shared_ptr<CameraBase> camera);
		static void SetEditorCamera(std::shared_ptr<EditorCamera> camera);
		static void SetRuntimeCamera(std::shared_ptr<CameraBase> camera);

		static std::shared_ptr<CameraBase> GetCameraUI();
		static std::shared_ptr<EditorCamera> GetEditorCamera();
		static std::shared_ptr<CameraBase> GetRuntimeCamera();
		static std::shared_ptr<CameraBase> GetActiveCamera();

		static void DrawModel(const std::shared_ptr<Model>& model, const std::shared_ptr<Material>& material, const glm::mat4& trs);
		static void Draw(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& trs, PassType passType = PassType::opaque, int sortOrder = 0);
		static void DrawInstanced(const std::shared_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& trs);

	protected:
		friend struct seri::Singleton<Graphic>;

		Graphic() = default;
		~Graphic() = default;

	private:
		std::shared_ptr<CameraBase> _cameraUI;
		std::shared_ptr<EditorCamera> _cameraEditor;
		std::shared_ptr<CameraBase> _cameraRuntime;

	};
}
