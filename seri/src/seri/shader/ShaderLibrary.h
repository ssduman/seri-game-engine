#pragma once

#include "seri/util/Util.h"
#include "seri/camera/CameraBase.h"
#include "seri/shader/ShaderBase.h"

#include <memory>
#include <vector>
#include <filesystem>

namespace seri
{
	class ShaderLibrary
	{
	public:

		struct ShaderInfo
		{
			bool valid;
			std::string name;
			std::string vsCode;
			std::string fsCode;
		};

		ShaderLibrary(ShaderLibrary const&) = delete;
		void operator=(ShaderLibrary const&) = delete;

		static ShaderLibrary& GetInstance();
		static void Init(const char* shaderFolderPath);

		static ShaderInfo& Get(const std::string& name);
		static std::shared_ptr<ShaderBase> Find(const std::string& name);

		static void SetView(std::shared_ptr<ShaderBase>& shader, const glm::mat4& view);
		static void SetProjection(std::shared_ptr<ShaderBase>& shader, const glm::mat4& projection);
		static void SetMVP(std::shared_ptr<ShaderBase>& shader, const std::shared_ptr<CameraBase>& camera);

	private:
		ShaderLibrary() = default;
		~ShaderLibrary() = default;

		std::unordered_map<std::string, ShaderInfo> _predefinedShaders;

	};
}
