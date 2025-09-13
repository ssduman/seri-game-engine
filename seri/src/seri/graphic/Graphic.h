#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <vector>
#include <memory>

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

		static void Draw(const std::unique_ptr<Mesh>& mesh, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera);

		static void DrawModel(const std::unique_ptr<Model>& model, const glm::mat4& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera);

		static void DrawInstanced(const std::unique_ptr<Mesh>& mesh, const std::vector<glm::mat4>& trs, std::shared_ptr<Material>& material, std::shared_ptr<CameraBase>& camera);

	private:
		Graphic() = default;

		~Graphic() = default;

		static void DrawElements(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
		{
			aux::DrawElement draw;
			draw.mode = aux::toGLenum(drawMode);
			draw.count = count;
			draw.type = aux::toGLenum(aux::Type::uint_type);
			draw.indices = nullptr;

			glDrawElements(draw.mode, draw.count, draw.type, draw.indices);
		}

		static void DrawElementsInstanced(GLsizei count, GLsizei instanceCount, aux::DrawMode drawMode = aux::DrawMode::triangles)
		{
			aux::DrawElement draw;
			draw.mode = aux::toGLenum(drawMode);
			draw.count = count;
			draw.type = aux::toGLenum(aux::Type::uint_type);
			draw.indices = nullptr;

			glDrawElementsInstanced(draw.mode, draw.count, draw.type, 0, instanceCount);
		}

		static void DrawArrays(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
		{
			aux::DrawArray draw;
			draw.mode = aux::toGLenum(drawMode);
			draw.first = 0;
			draw.count = count;

			glDrawArrays(draw.mode, draw.first, draw.count);
		}

		std::shared_ptr<CameraBase> _cameraOrtho;
		std::shared_ptr<CameraBase> _cameraPerspective;
		std::vector<std::shared_ptr<CameraBase>> _cameras;

	};
}
