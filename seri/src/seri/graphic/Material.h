#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/texture/TextureBase.h"

namespace seri
{
	class Material
	{
	public:
		friend struct MaterialAsset;

		void SetInt(const std::string& name, int v);

		void SetFloat(const std::string& name, float v);

		void SetFloat2(const std::string& name, const glm::vec2& v);

		void SetFloat3(const std::string& name, const glm::vec3& v);

		void SetFloat4(const std::string& name, const glm::vec4& v);

		void SetMat4(const std::string& name, const glm::mat4& m);

		void SetTexture(const std::string& name, std::shared_ptr<TextureBase> tex);

		void Apply();

		void UnbindTextures();

		uint64_t id{ 0 };
		uint64_t shaderID{ 0 };

		std::shared_ptr<ShaderBase> shader;

		std::unordered_map<std::string, int>& GetInts() { return _ints; }
		std::unordered_map<std::string, float>& GetFloats() { return _floats; }
		std::unordered_map<std::string, glm::vec2>& GetFloat2s() { return _float2s; }
		std::unordered_map<std::string, glm::vec3>& GetFloat3s() { return _float3s; }
		std::unordered_map<std::string, glm::vec4>& GetFloat4s() { return _float4s; }
		std::unordered_map<std::string, glm::mat4>& GetMats() { return _mats; }
		std::unordered_map<std::string, std::shared_ptr<TextureBase>>& GetTextures() { return _textures; }

	private:
		std::unordered_map<std::string, int> _ints;
		std::unordered_map<std::string, float> _floats;
		std::unordered_map<std::string, glm::vec2> _float2s;
		std::unordered_map<std::string, glm::vec3> _float3s;
		std::unordered_map<std::string, glm::vec4> _float4s;
		std::unordered_map<std::string, glm::mat4> _mats;

		std::unordered_map<std::string, std::shared_ptr<TextureBase>> _textures;

	};
}
