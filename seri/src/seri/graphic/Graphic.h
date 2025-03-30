#pragma once

#include "seri/core/Seri.h"
#include "seri/camera/ICamera.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <vector>
#include <memory>

class MeshG;
class MaterialG;

class Graphic
{
public:
	Graphic(Graphic const&) = delete;

	void operator=(Graphic const&) = delete;

	static void Init()
	{
		GetInstance();
		LOGGER(info, "graphic manager init done");
	}

	static Graphic& GetInstance()
	{
		static Graphic instance;
		return instance;
	}

	static void AddCamera(std::shared_ptr<ICamera> camera);

	static std::shared_ptr<ICamera> GetCameraOrtho();

	static std::shared_ptr<ICamera> GetCameraPerspective();

	static void Draw(std::shared_ptr<MeshG> mesh, const glm::mat4& trs, std::shared_ptr<MaterialG> material, std::shared_ptr<ICamera> camera);

private:
	Graphic()
	{
		LOGGER(info, "graphic manager init");
	}

	~Graphic()
	{
		LOGGER(info, "graphic manager release");
	}

	static void drawElements(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
	{
		aux::DrawElement draw;
		draw.mode = aux::toGLenum(drawMode);
		draw.count = count;
		draw.type = aux::toGLenum(aux::Type::uint_type);
		draw.indices = nullptr;

		glDrawElements(draw.mode, draw.count, draw.type, draw.indices);
	}

	static void drawArrays(GLsizei count, aux::DrawMode drawMode = aux::DrawMode::triangles)
	{
		aux::DrawArray draw;
		draw.mode = aux::toGLenum(drawMode);
		draw.first = 0;
		draw.count = count;

		glDrawArrays(draw.mode, draw.first, draw.count);
	}

	std::shared_ptr<ICamera> _cameraOrtho;
	std::shared_ptr<ICamera> _cameraPerspective;
	std::vector<std::shared_ptr<ICamera>> _cameras;

};
