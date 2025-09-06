#pragma once

#include "seri/util/Util.h"

namespace seri
{
	enum class LayoutLocation
	{
		none = -1,
		position = 0,
		uv0 = 1,
		color = 2,
		normal = 3,
		skin_bone_id = 4,
		skin_weight = 5,
		instanced_mat4 = 6,
		instanced_mat4_reserved_1 = 7,
		instanced_mat4_reserved_2 = 8,
		instanced_mat4_reserved_3 = 9,

		loc_0 = 0,
		loc_1 = 1,
		loc_2 = 2,
		loc_3 = 3,
		loc_4 = 4,
		loc_5 = 5,
		loc_6 = 6,
		loc_7 = 7,
		loc_8 = 8,
		loc_9 = 9,
		loc_10 = 10,
		loc_11 = 11,
		loc_12 = 12,
		loc_13 = 13,
		loc_14 = 14,
		loc_15 = 15,
	};

	enum class ShaderDataType
	{
		int_type,
		int2_type,
		int3_type,
		int4_type,
		float_type,
		float2_type,
		float3_type,
		float4_type,
		mat3_type,
		mat4_type,
	};

	struct BufferElementDesc
	{
		friend struct BufferLayoutDesc;

		std::string name;
		LayoutLocation loc;
		ShaderDataType type;
		bool normalized;

		BufferElementDesc(const std::string& name, ShaderDataType type, bool normalized = false)
		{
			this->name = name;
			this->type = type;
			this->normalized = normalized;

			loc = LayoutLocation::none;

			offset = 0;
			size = GetShaderDataTypeSize(type);
			count = GetShaderDataTypeCount(type);
		}

		BufferElementDesc(LayoutLocation loc, ShaderDataType type, bool normalized = false)
		{
			this->loc = loc;
			this->type = type;
			this->normalized = normalized;

			name = "";

			offset = 0;
			size = GetShaderDataTypeSize(type);
			count = GetShaderDataTypeCount(type);
		}

		inline uint32_t GetCount() const
		{
			return count;
		}

		inline uint32_t GetOffset() const
		{
			return offset;
		}

	private:
		uint32_t GetShaderDataTypeSize(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::int_type: return 4;
				case ShaderDataType::int2_type: return 4 * 2;
				case ShaderDataType::int3_type: return 4 * 3;
				case ShaderDataType::int4_type: return 4 * 4;
				case ShaderDataType::float_type: return 4;
				case ShaderDataType::float2_type: return 4 * 2;
				case ShaderDataType::float3_type: return 4 * 3;
				case ShaderDataType::float4_type: return 4 * 4;
				case ShaderDataType::mat3_type: return 4 * 3 * 3;
				case ShaderDataType::mat4_type: return 4 * 4 * 4;
			}

			return 0;
		}

		uint32_t GetShaderDataTypeCount(ShaderDataType type)
		{
			switch (type)
			{
				case ShaderDataType::int_type: return 1;
				case ShaderDataType::int2_type: return 2;
				case ShaderDataType::int3_type: return 3;
				case ShaderDataType::int4_type: return 4;
				case ShaderDataType::float_type: return 1;
				case ShaderDataType::float2_type: return 2;
				case ShaderDataType::float3_type: return 3;
				case ShaderDataType::float4_type: return 4;
				case ShaderDataType::mat3_type: return 3; // 3 * float3
				case ShaderDataType::mat4_type: return 4; // 4 * float4
			}

			return 0;
		}

		uint32_t offset; // [vert0, vert1, vert2, color0, color1, color2] -> offset 0 and offset 3 for two elements
		uint32_t size; // 3 * sizeof(float) for float3 -> [vert0, vert1, vert2]
		uint32_t count; // 3 for float3

	};

	struct BufferLayoutDesc
	{
		BufferLayoutDesc() = default;

		BufferLayoutDesc(std::initializer_list<BufferElementDesc> elements)
		{
			for (const auto& element : elements)
			{
				AddElement(element);
			}
		}

		BufferLayoutDesc& AddElement(BufferElementDesc desc)
		{
			_elements.emplace_back(desc);
			CalculateStrideAndOffset();
			return *this;
		}

		uint32_t GetStride() const
		{
			return _stride;
		}

		const std::vector<BufferElementDesc>& GetElements() const
		{
			return _elements;
		}

	private:
		void CalculateStrideAndOffset()
		{
			_stride = 0;
			size_t offset = 0;
			for (auto& element : _elements)
			{
				element.offset = offset;
				offset += element.size;
				_stride += element.size;
			}
		}

		uint32_t _stride = 0; // [vert0, vert1, vert2, color0, color1, color2] -> stride 6
		std::vector<BufferElementDesc> _elements;

	};

}
