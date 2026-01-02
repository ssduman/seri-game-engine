#include "Seripch.h"

#include "seri/graphic/Material.h"

namespace seri
{
	void Material::SetBool(const std::string& name, bool v)
	{
		_bools[name] = v;
	}

	void Material::SetInt(const std::string& name, int v)
	{
		_ints[name] = v;
	}

	void Material::SetInt2(const std::string& name, const glm::ivec2& v)
	{
		_int2s[name] = v;
	}

	void Material::SetInt3(const std::string& name, const glm::ivec3& v)
	{
		_int3s[name] = v;
	}

	void Material::SetInt4(const std::string& name, const glm::ivec4& v)
	{
		_int4s[name] = v;
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

	void Material::SetShader(std::shared_ptr<ShaderBase> shader)
	{
		_shader = shader;

		ClearAll();

		for (const auto& uniform : _shader->GetUniforms())
		{
			switch (uniform.type)
			{
				case UniformType::bool_type:
					SetBool(uniform.name, false);
					break;
				case UniformType::int_type:
					SetInt(uniform.name, 0);
					break;
				case UniformType::ivec2_type:
					SetInt2(uniform.name, glm::ivec2(0));
					break;
				case UniformType::ivec3_type:
					SetInt3(uniform.name, glm::ivec3(0));
					break;
				case UniformType::ivec4_type:
					SetInt4(uniform.name, glm::ivec4(0));
					break;
				case UniformType::float_type:
					SetFloat(uniform.name, 0.0f);
					break;
				case UniformType::vec2_type:
					SetFloat2(uniform.name, glm::vec2(0.0f));
					break;
				case UniformType::vec3_type:
					SetFloat3(uniform.name, glm::vec3(0.0f));
					break;
				case UniformType::vec4_type:
					SetFloat4(uniform.name, glm::vec4(0.0f));
					break;
				case UniformType::sampler2d_type:
					SetTexture(uniform.name, nullptr);
					break;
				default:
					break;
			}
		}
	}

	void Material::Apply()
	{
		if (!_shader || !_shader->IsActiveForUsing())
		{
			return;
		}

		_shader->Bind();

		for (auto& kv : _bools)
		{
			_shader->SetBool(kv.first, kv.second);
		}

		for (auto& kv : _ints)
		{
			_shader->SetInt(kv.first, kv.second);
		}

		for (auto& kv : _int2s)
		{
			_shader->SetInt2(kv.first, kv.second);
		}

		for (auto& kv : _int3s)
		{
			_shader->SetInt3(kv.first, kv.second);
		}

		for (auto& kv : _int4s)
		{
			_shader->SetInt4(kv.first, kv.second);
		}

		for (auto& kv : _floats)
		{
			_shader->SetFloat(kv.first, kv.second);
		}

		for (auto& kv : _float2s)
		{
			_shader->SetFloat2(kv.first, kv.second);
		}

		for (auto& kv : _float3s)
		{
			_shader->SetFloat3(kv.first, kv.second);
		}

		for (auto& kv : _float4s)
		{
			_shader->SetFloat4(kv.first, kv.second);
		}

		for (auto& kv : _mats)
		{
			_shader->SetMat4(kv.first, kv.second);
		}

		for (auto& kv : _textures)
		{
			const std::string& name = kv.first;
			const std::shared_ptr<TextureBase>& tex = kv.second;

			if (!tex || !tex->IsActiveForUsing())
			{
				continue;
			}

			int slot = _shader->GetTextureSlot(name);
			if (slot < 0)
			{
				continue;
			}

			tex->Bind(slot);
			_shader->SetInt(name, slot);
		}
	}

	void Material::ClearAll()
	{
		_bools.clear();
		_ints.clear();
		_int2s.clear();
		_int3s.clear();
		_int4s.clear();
		_floats.clear();
		_float2s.clear();
		_float3s.clear();
		_float4s.clear();
		_mats.clear();
		_textures.clear();
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
