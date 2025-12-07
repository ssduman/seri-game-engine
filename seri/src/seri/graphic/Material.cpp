#include "Seripch.h"

#include "seri/graphic/Material.h"

namespace seri
{
	void Material::SetInt(const std::string& name, int v)
	{
		_ints[name] = v;
	}

	void Material::SetFloat(const std::string& name, float v)
	{
		_floats[name] = v;
	}

	void Material::SetFloat2(const std::string& name, const glm::vec2& v)
	{
		_float2s[name] = v;
	}

	void Material::SetFloat3(const std::string& name, const glm::vec3& v)
	{
		_float3s[name] = v;
	}

	void Material::SetFloat4(const std::string& name, const glm::vec4& v)
	{
		_float4s[name] = v;
	}

	void Material::SetMat4(const std::string& name, const glm::mat4& m)
	{
		_mats[name] = m;
	}

	void Material::SetTexture(const std::string& name, std::shared_ptr<TextureBase> tex)
	{
		_textures[name] = tex;
	}

	void Material::Apply()
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

	void Material::UnbindTextures()
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

}
