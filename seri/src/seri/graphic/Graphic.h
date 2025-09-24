#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/rendering/FramebufferBase.h"

namespace seri
{
	class Mesh;
	class Model;
	class Material;

	class Graphic
	{
	public:
		Graphic(Graphic const&) = delete;

		void operator=(Graphic const&) = delete;

		static Graphic& GetInstance()
		{
			static Graphic instance;
			return instance;
		}

		static void Init()
		{
			GetInstance();
		}

		static void AddCamera(std::shared_ptr<CameraBase> camera);

		static std::shared_ptr<CameraBase> GetCameraOrtho();

		static std::shared_ptr<CameraBase> GetCameraPerspective();

		static void DrawModel(const std::unique_ptr<Model>& model, const std::shared_ptr<Material>& material, const glm::mat4& trs, const std::shared_ptr<CameraBase>& camera, const std::shared_ptr<FramebufferBase>& rt = nullptr);

		static void Draw(const std::unique_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const glm::mat4& trs, const std::shared_ptr<CameraBase>& camera, const std::shared_ptr<FramebufferBase>& rt = nullptr);

		static void DrawInstanced(const std::unique_ptr<Mesh>& mesh, const std::shared_ptr<Material>& material, const std::vector<glm::mat4>& trs, const std::shared_ptr<CameraBase>& camera);

	private:
		Graphic() = default;

		~Graphic() = default;

		std::shared_ptr<CameraBase> _cameraOrtho;
		std::shared_ptr<CameraBase> _cameraPerspective;
		std::vector<std::shared_ptr<CameraBase>> _cameras;

	};
}
