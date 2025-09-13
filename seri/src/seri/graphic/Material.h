#pragma once

#include "seri/util/Util.h"
#include "seri/shader/ShaderBase.h"
#include "seri/texture/TextureBase.h"

namespace seri
{
	class Material
	{
	public:
		Material() = default;

		~Material() = default;

		std::shared_ptr<ShaderBase> shader;

		std::shared_ptr<TextureBase> texture;

		void SetInt(const std::string& name, int v)
		{
			_ints[name] = v;
		}

		void SetFloat(const std::string& name, float v)
		{
			_floats[name] = v;
		}

		void SetFloat2(const std::string& name, const glm::vec2& v)
		{
			_float2s[name] = v;
		}

		void SetFloat3(const std::string& name, const glm::vec3& v)
		{
			_float3s[name] = v;
		}

		void SetFloat4(const std::string& name, const glm::vec4& v)
		{
			_float4s[name] = v;
		}

		void SetMat4(const std::string& name, const glm::mat4& m)
		{
			_mats[name] = m;
		}

		void SetTexture(const std::string& name, std::shared_ptr<TextureBase> tex)
		{
			_textures[name] = tex;
		}

		void Apply()
		{
			if (!shader || !shader->IsActiveForUsing())
			{
				return;
			}

			shader->Bind();

			for (auto& kv : _textures)
			{
				const std::string& name = kv.first;
				const std::shared_ptr<TextureBase>& tex = kv.second;

				if (!tex || !tex->IsActiveForUsing())
				{
					continue;
				}

				tex->Bind();
				shader->SetInt(name, tex->GetSlot());
			}

			for (auto& kv : _ints)
			{
				shader->SetInt(kv.first, kv.second);
			}

			for (auto& kv : _floats)
			{
				shader->SetFloat(kv.first, kv.second);
			}

			for (auto& kv : _float2s)
			{
				shader->SetFloat2(kv.first, kv.second);
			}

			for (auto& kv : _float3s)
			{
				shader->SetFloat3(kv.first, kv.second);
			}

			for (auto& kv : _float4s)
			{
				shader->SetFloat4(kv.first, kv.second);
			}

			for (auto& kv : _mats)
			{
				shader->SetMat4(kv.first, kv.second);
			}
		}

		void UnbindTextures()
		{
			for (auto& kv : _textures)
			{
				const std::shared_ptr<TextureBase>& tex = kv.second;

				if (!tex || !tex->IsActiveForUsing())
				{
					continue;
				}

				tex->Unbind();
			}
		}

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
