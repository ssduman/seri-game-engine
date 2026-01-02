#pragma once

#include "seri/util/Util.h"
#include "seri/random/Random.h"
#include "seri/asset/AssetBase.h"
#include "seri/shader/ShaderBase.h"
#include "seri/texture/TextureBase.h"

namespace seri
{
	class Material : public seri::asset::AssetBase
	{
	public:
		Material()
		{
			id = seri::Random::UUID();
			type = seri::asset::AssetType::material;
		}

		Material(uint64_t id_)
		{
			type = seri::asset::AssetType::material;
		}

		void SetBool(const std::string& name, bool v);

		void SetInt(const std::string& name, int v);
		void SetInt2(const std::string& name, const glm::ivec2& v);
		void SetInt3(const std::string& name, const glm::ivec3& v);
		void SetInt4(const std::string& name, const glm::ivec4& v);

		void SetFloat(const std::string& name, float v);
		void SetFloat2(const std::string& name, const glm::vec2& v);
		void SetFloat3(const std::string& name, const glm::vec3& v);
		void SetFloat4(const std::string& name, const glm::vec4& v);

		void SetMat4(const std::string& name, const glm::mat4& m);

		void SetTexture(const std::string& name, std::shared_ptr<TextureBase> tex);

		void SetShader(std::shared_ptr<ShaderBase> shader);

		void Apply();
		void ClearAll();
		void UnbindTextures();

		std::shared_ptr<ShaderBase> GetShader() { return _shader; }

		std::unordered_map<std::string, bool>& GetBools() { return _bools; }

		std::unordered_map<std::string, int>& GetInts() { return _ints; }
		std::unordered_map<std::string, glm::ivec2>& GetInt2s() { return _int2s; }
		std::unordered_map<std::string, glm::ivec3>& GetInt3s() { return _int3s; }
		std::unordered_map<std::string, glm::ivec4>& GetInt4s() { return _int4s; }

		std::unordered_map<std::string, float>& GetFloats() { return _floats; }
		std::unordered_map<std::string, glm::vec2>& GetFloat2s() { return _float2s; }
		std::unordered_map<std::string, glm::vec3>& GetFloat3s() { return _float3s; }
		std::unordered_map<std::string, glm::vec4>& GetFloat4s() { return _float4s; }

		std::unordered_map<std::string, glm::mat4>& GetMats() { return _mats; }

		std::unordered_map<std::string, std::shared_ptr<TextureBase>>& GetTextures() { return _textures; }

		struct TextureSlotInfo
		{
			uint64_t id;
			std::string name;
		};
		uint64_t shaderID{ 0 };
		std::vector<TextureSlotInfo> textureIDs{};

	private:
		std::shared_ptr<ShaderBase> _shader;

		std::unordered_map<std::string, bool> _bools;

		std::unordered_map<std::string, int> _ints;
		std::unordered_map<std::string, glm::ivec2> _int2s;
		std::unordered_map<std::string, glm::ivec3> _int3s;
		std::unordered_map<std::string, glm::ivec4> _int4s;

		std::unordered_map<std::string, float> _floats;
		std::unordered_map<std::string, glm::vec2> _float2s;
		std::unordered_map<std::string, glm::vec3> _float3s;
		std::unordered_map<std::string, glm::vec4> _float4s;
		std::unordered_map<std::string, glm::mat4> _mats;

		std::unordered_map<std::string, std::shared_ptr<TextureBase>> _textures;

	};
}
