#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
#include "seri/asset/AssetBase.h"

namespace seri
{
	class ShaderBase : public seri::asset::AssetBase
	{
	public:
		ShaderBase()
		{
			id = seri::Random::UUID();
			type = seri::asset::AssetType::shader;
		}

		ShaderBase(uint64_t id_)
		{
			id = id_;
			type = seri::asset::AssetType::shader;
		}

		virtual void Init(const std::string& shaderPath) = 0;
		virtual void Init(const char* vsCodePath, const char* fsCodePath, bool readFromFile = true) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Release() = 0;

		virtual bool IsActiveForUsing() = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetBool(const std::string& name, bool value) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static std::shared_ptr<ShaderBase> Create();

	};
}
