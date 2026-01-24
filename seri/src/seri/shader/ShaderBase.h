#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
#include "seri/asset/AssetBase.h"
#include "seri/rendering/render/RenderingUtil.h"

namespace seri
{
	struct ShaderUniform
	{
		std::string name;
		UniformType type;
		int32_t slot;
		uint32_t location;
	};

	class ShaderBase : public seri::asset::AssetBase
	{
	public:
		class ShaderLibrary;

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
		virtual void Init(const std::string& vsCode, const std::string& fsCode) = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void Release() = 0;

		virtual bool IsActiveForUsing() = 0;

		virtual void SetBool(const std::string& name, bool value) = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetInt2(const std::string& name, const glm::ivec2& value) = 0;
		virtual void SetInt3(const std::string& name, const glm::ivec3& value) = 0;
		virtual void SetInt4(const std::string& name, const glm::ivec4& value) = 0;

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		virtual std::shared_ptr<ShaderBase> Clone() = 0;

		std::vector<ShaderUniform> GetUniforms()
		{
			return _uniforms;
		}

		int GetTextureSlot(std::string_view name)
		{
			for (const auto& uniform : _uniforms)
			{
				if (uniform.name == name)
				{
					return uniform.slot;
				}
			}

			return -1;
		}

		static std::shared_ptr<ShaderBase> Create();

	protected:
		virtual void Build(const std::string& vsCode, const std::string& fsCode) = 0;

		virtual void ParseUniforms() = 0;

		std::string _vsCode{ "" };
		std::string _fsCode{ "" };

		std::vector<ShaderUniform> _uniforms{};

	};
}
