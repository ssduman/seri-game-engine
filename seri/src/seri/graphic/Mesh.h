#pragma once

#include "seri/util/Util.h"
#include "seri/rendering/RenderingManager.h"

#define SERI_MAX_BONES 200
#define SERI_MAX_INSTANCED_COUNT 256
#define SERI_MAX_NUM_BONES_PER_VERTEX 4

namespace seri
{
	struct TangentData
	{
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};

	struct Bone
	{
		unsigned int index{};
		std::string name;
		glm::mat4 offsetMatrix{};
		glm::mat4 transform{ 1.0f };
	};

	struct NodeData
	{
		std::string name;
		glm::mat4 transformation{};
		std::vector<NodeData> children;
	};

	struct VertexBoneData
	{
	public:
		VertexBoneData() = default;

		~VertexBoneData() = default;

		void AddBoneData(unsigned int boneId, float weight)
		{
			if (_count >= SERI_MAX_NUM_BONES_PER_VERTEX)
			{
				return;
			}

			if (weight < 0.001f)
			{
				return;
			}

			for (int i = 0; i < _count; i++)
			{
				if (boneIds[i] == boneId)
				{
					return;
				}
			}

			boneIds[_count] = boneId;
			weights[_count] = weight;

			_count += 1;
		}

		unsigned int boneIds[SERI_MAX_NUM_BONES_PER_VERTEX] = { 0 };
		float weights[SERI_MAX_NUM_BONES_PER_VERTEX] = { 0.0f };

	private:
		int _count = 0;

	};

	struct AnimPositionKey
	{
		double timeInTick;
		glm::vec3 position;
	};

	struct AnimRotationKey
	{
		double timeInTick;
		glm::quat quaternion;
	};

	struct AnimScaleKey
	{
		double timeInTick;
		glm::vec3 scale;
	};

	struct NodeAnimation
	{
		std::string nodeName;

		std::vector<AnimPositionKey> positions{};
		std::vector<AnimRotationKey> rotations{};
		std::vector<AnimScaleKey> scales{};
	};

	struct Animation
	{
		std::string name;
		double durationInTick{};
		double tickPerSecond{};
		std::unordered_map<std::string, NodeAnimation> nodeAnimations;
	};

	class Mesh
	{
	public:
		std::vector<uint32_t> indices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uv0s;
		std::vector<glm::vec2> uv1s;
		std::vector<glm::vec2> uv2s;
		std::vector<glm::vec2> uv3s;
		std::vector<glm::vec2> uv4s;
		std::vector<glm::vec2> uv5s;
		std::vector<glm::vec2> uv6s;
		std::vector<glm::vec2> uv7s;
		std::vector<glm::vec3> colors;
		std::vector<glm::vec3> normals;
		std::vector<TangentData> tangentData;

		NodeData nodeData{};
		Animation animation{};

		std::unordered_map<int, Bone> bones{};
		std::vector<VertexBoneData> bonesForVertices{};
		std::vector<glm::vec3> blendShapes{};

		std::string name{};
		std::string nodeName{};
		std::unordered_map<std::string, int> boneNameToIndexMap{};

		glm::mat4 transformation{ 1.0f };

		int materialIndex{ 0 };

		void AddIndices(std::vector<unsigned int> ind)
		{
			indices.insert(indices.end(), ind.begin(), ind.end());
		}

		void AddVertices(std::vector<glm::vec3> ver)
		{
			vertices.insert(vertices.end(), ver.begin(), ver.end());
		}

		void AddColors(std::vector<glm::vec4> col)
		{
			colors.insert(colors.end(), col.begin(), col.end());
		}

		void AddUV(std::vector<glm::vec2> uv)
		{
			uv0s.insert(uv0s.end(), uv.begin(), uv.end());
		}

		void AddNormals(std::vector<glm::vec3> nor)
		{
			normals.insert(normals.end(), nor.begin(), nor.end());
		}

		void AddTangentsAndBitangents(std::vector<TangentData> data)
		{
			tangentData.insert(tangentData.end(), data.begin(), data.end());
		}

		void UpdateAnimation(double time)
		{
			auto timeInTicks = time * animation.tickPerSecond;

			animTimeInTick = std::fmod(timeInTicks, animation.durationInTick);
			animTime = static_cast<float>(animTimeInTick) / static_cast<float>(animation.tickPerSecond);
			//LOGGER(info, "[mesh] time: " << time << ", localAnimationTime: " << localAnimationTime);

			UpdateAnimation(nodeData, glm::mat4{ 1.0f });
		}

		void UpdateAnimation(const NodeData& node, const glm::mat4& parentTransform)
		{
			std::string nodeName = node.name;
			glm::mat4 nodeTransform = node.transformation;
			glm::mat4 trs = glm::mat4{ 1.0f };

			int boneIndex = -1;

			{
				if (animation.nodeAnimations.find(nodeName) != animation.nodeAnimations.end())
				{
					const NodeAnimation& nodeAnim = animation.nodeAnimations[nodeName];

					if (nodeAnim.nodeName != nodeName)
					{
						LOGGER(error, "[mesh] bone anim name mismatch: " << nodeAnim.nodeName << ", " << nodeName);
					}

					if (bones.size() > 0)
					{
						if (boneNameToIndexMap.find(nodeName) != boneNameToIndexMap.end())
						{
							boneIndex = boneNameToIndexMap[nodeName];

							glm::vec3 position = InterpolatePosition(nodeAnim);
							glm::quat rotation = InterpolateRotation(nodeAnim);
							glm::vec3 scale = InterpolateScale(nodeAnim);

							trs = Util::GetTRS(position, rotation, scale);
						}
					}
					else
					{
						if (this->nodeName == nodeName)
						{
							glm::vec3 position = InterpolatePosition(nodeAnim);
							glm::quat rotation = InterpolateRotation(nodeAnim);
							glm::vec3 scale = InterpolateScale(nodeAnim);

							trs = Util::GetTRS(position, rotation, scale);
						}
					}
				}
			}

			glm::mat4 globalTransform = parentTransform * trs;

			if (bones.size() > 0)
			{
				if (boneIndex != -1 && bones.find(boneIndex) != bones.end())
				{
					if (bones.at(boneIndex).name != nodeName)
					{
						LOGGER(error, "[mesh] bone name mismatch: " << nodeName << ", " << bones.at(boneIndex).name);
					}
					else
					{
						bones.at(boneIndex).transform = globalTransform * bones.at(boneIndex).offsetMatrix;
					}
				}
			}
			else
			{
				if (this->nodeName == nodeName)
				{
					transformation = globalTransform;
				}
			}

			for (const NodeData& child : node.children)
			{
				UpdateAnimation(child, globalTransform);
			}
		}

		void Build()
		{
			if (_vao)
			{
				//LOGGER(warning, "[mesh] vao already generated for build");
				return;
			}

			_vao = VertexArrayBase::Create();

			if (indices.size() > 0)
			{
				_ebo = IndexBufferBase::Create(indices);

				_vao->SetIndexBuffer(_ebo);
			}

			if (vertices.size() > 0)
			{
				_vbo_ver = VertexBufferBase::Create(vertices);
				_vbo_ver->AddElement(
					{ seri::LayoutLocation::vertex }
				);

				_vao->AddVertexBuffer(_vbo_ver);
			}

			if (uv0s.size() > 0)
			{
				_vbo_uv0 = VertexBufferBase::Create(uv0s);
				_vbo_uv0->AddElement(
					{ seri::LayoutLocation::uv0 }
				);

				_vao->AddVertexBuffer(_vbo_uv0);
			}

			if (colors.size() > 0)
			{
				_vbo_col = VertexBufferBase::Create(colors);
				_vbo_col->AddElement(
					{ seri::LayoutLocation::color }
				);

				_vao->AddVertexBuffer(_vbo_col);
			}

			if (normals.size() > 0)
			{
				_vbo_nor = VertexBufferBase::Create(normals);
				_vbo_nor->AddElement(
					{ seri::LayoutLocation::normal }
				);

				_vao->AddVertexBuffer(_vbo_nor);
			}

			if (tangentData.size() > 0)
			{
				_vbo_tan = VertexBufferBase::Create(tangentData.data(), tangentData.size() * sizeof(TangentData));
				_vbo_tan->AddElements(
					{
						{ seri::LayoutLocation::tangent },
						{ seri::LayoutLocation::bitangent }
					}
				);

				_vao->AddVertexBuffer(_vbo_tan);
			}

			if (bonesForVertices.size() > 0)
			{
				_vbo_skin = VertexBufferBase::Create(bonesForVertices.data(), bonesForVertices.size() * sizeof(VertexBoneData));
				_vbo_skin->AddElement(
					{ seri::LayoutLocation::skin_bone_id, ShaderDataType::int4_type }
				);
				_vbo_skin->AddElement(
					{ seri::LayoutLocation::skin_weight, ShaderDataType::float4_type }
				);
				_vbo_skin->AddElement(
					{ seri::LayoutLocation::none, ShaderDataType::int_type }
				);

				_vao->AddVertexBuffer(_vbo_skin);
			}
		}

		void Update()
		{
			if (!_vao)
			{
				Build();
				//LOGGER(error, "[mesh] vao for update not generated");
				return;
			}

			if (_ebo)
			{
				_ebo->SetData(indices);
			}

			if (_vbo_ver)
			{
				_vbo_ver->SetData(vertices);
			}

			if (_vbo_uv0)
			{
				_vbo_uv0->SetData(uv0s);
			}

			if (_vbo_col)
			{
				_vbo_col->SetData(colors);
			}

			if (_vbo_nor)
			{
				_vbo_nor->SetData(normals);
			}

			if (_vbo_skin)
			{
				_vbo_skin->SetData(&bonesForVertices[0], bonesForVertices.size() * sizeof(VertexBoneData));
			}
		}

		void Clear()
		{
			indices.clear();
			vertices.clear();
			uv0s.clear();
			uv1s.clear();
			uv2s.clear();
			uv3s.clear();
			uv4s.clear();
			uv5s.clear();
			uv6s.clear();
			uv7s.clear();
			colors.clear();
			normals.clear();
			tangentData.clear();
		}

		void MakeInstanced(const std::vector<glm::mat4>& modelMatrices)
		{
			if (modelMatrices.size() > SERI_MAX_INSTANCED_COUNT)
			{
				LOGGER(error, "[mesh] model matrices size exceeds max instanced count");
				return;
			}

			if (!_vao)
			{
				LOGGER(error, "[mesh] vao for instanced not generated");
				return;
			}

			if (_vbo_instanced)
			{
				LOGGER(error, "[mesh] vbo for instanced already generated");
				return;
			}

			_vbo_instanced = VertexBufferBase::Create(modelMatrices);
			_vbo_instanced->AddElement(
				{ seri::LayoutLocation::instanced_mat4 }
			);

			_vao->AddVertexBuffer(_vbo_instanced);
		}

		void UpdateInstanced(const std::vector<glm::mat4>& modelMatrices)
		{
			if (modelMatrices.size() > SERI_MAX_INSTANCED_COUNT)
			{
				LOGGER(error, "[mesh] model matrices size exceeds max instanced count");
				return;
			}

			if (!_vao)
			{
				LOGGER(error, "[mesh] vao for instanced not generated");
				return;
			}

			if (!_vbo_instanced)
			{
				LOGGER(error, "[mesh] vbo for instanced not generated");
				return;
			}

			_vbo_instanced->UpdateData(modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));
		}

		void Bind()
		{
			_vao->Bind();
		}

		void Unbind()
		{
			_vao->Unbind();
		}

		std::shared_ptr<VertexArrayBase> GetVao()
		{
			return _vao;
		}

		static std::shared_ptr<Mesh> tri_2d()
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

			mesh->vertices = {
				{0.0f, 0.0f, 0.0f},
				{1.0f, 1.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
			};

			mesh->uv0s = {
				{0.0f, 0.0f},
				{1.0f, 1.0f},
				{1.0f, 0.0f},
				{0.0f, 1.0f},
			};

			mesh->indices = {
				0, 1, 2,
			};

			mesh->Build();

			return mesh;
		}

		static std::shared_ptr<Mesh> quad_2d()
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

			mesh->vertices = {
				{-1.0f, -1.0f, 0.0f},
				{+1.0f, -1.0f, 0.0f},
				{+1.0f, +1.0f, 0.0f},
				{-1.0f, +1.0f, 0.0f},
			};

			mesh->uv0s = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
			};

			mesh->indices = {
				0, 1, 3,
				1, 2, 3,
			};

			mesh->Build();

			return mesh;
		}

		static std::shared_ptr<Mesh> quad_3d()
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

			mesh->vertices = {
				{0.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
				{1.0f, 1.0f, 0.0f},
				{0.0f, 1.0f, 0.0f},
			};

			mesh->uv0s = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f},
			};

			mesh->indices = {
				0, 1, 3,
				1, 2, 3,
			};

			mesh->Build();

			return mesh;
		}

		static std::shared_ptr<Mesh> cube_3d(float mult = 0.4f)
		{
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

			mesh->vertices = {
				{ -0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, +0.5f * mult },
				{ -0.5f * mult, +0.5f * mult, +0.5f * mult },

				{ -0.5f * mult, -0.5f * mult, -0.5f * mult },
				{ +0.5f * mult, -0.5f * mult, -0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, -0.5f * mult },
				{ -0.5f * mult, +0.5f * mult, -0.5f * mult },

				{ -0.5f * mult, -0.5f * mult, -0.5f * mult },
				{ -0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ -0.5f * mult, +0.5f * mult, +0.5f * mult },
				{ -0.5f * mult, +0.5f * mult, -0.5f * mult },

				{ +0.5f * mult, -0.5f * mult, -0.5f * mult },
				{ +0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, -0.5f * mult },

				{ -0.5f * mult, +0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, +0.5f * mult, -0.5f * mult },
				{ -0.5f * mult, +0.5f * mult, -0.5f * mult },

				{ -0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, -0.5f * mult, +0.5f * mult },
				{ +0.5f * mult, -0.5f * mult, -0.5f * mult },
				{ -0.5f * mult, -0.5f * mult, -0.5f * mult },
			};

			mesh->normals = {
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },
				{  0.0f,  0.0f,  1.0f },

				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },
				{  0.0f,  0.0f, -1.0f },

				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },
				{ -1.0f,  0.0f,  0.0f },

				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },
				{ +1.0f,  0.0f,  0.0f },

				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },
				{  0.0f, +1.0f,  0.0f },

				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
				{  0.0f, -1.0f,  0.0f },
			};

			mesh->uv0s = {
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f },
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },

				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 0.0f },
				{ 0.0f, 0.0f },
				{ 0.0f, 1.0f },
				{ 1.0f, 1.0f },

				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },

				{ 1.0f, 1.0f },
				{ 0.0f, 1.0f },
				{ 0.0f, 0.0f },
				{ 1.0f, 0.0f },
			};

			mesh->indices = {
				0, 1, 2,
				0, 2, 3,

				4, 6, 5,
				4, 7, 6,

				8, 10, 9,
				8, 11, 10,

				12, 13, 14,
				12, 14, 15,

				16, 17, 18,
				16, 18, 19,

				20, 23, 22,
				20, 22, 21,
			};

			mesh->Build();

			return mesh;
		}

	private:
		glm::vec3 InterpolatePosition(const NodeAnimation& nodeAnim)
		{
			if (nodeAnim.positions.empty())
			{
				return glm::vec3{ 0.0f, 0.0f, 0.0f };
			}

			if (nodeAnim.positions.size() == 1)
			{
				return nodeAnim.positions[0].position;
			}

			unsigned int prevIndex = 0;
			for (unsigned int i = 0; i < nodeAnim.positions.size() - 1; i++)
			{
				if (animTimeInTick < nodeAnim.positions[i + 1].timeInTick)
				{
					prevIndex = i;
					break;
				}
			}

			unsigned int nextIndex = prevIndex + 1;

			auto t1 = nodeAnim.positions[prevIndex].timeInTick;
			auto t2 = nodeAnim.positions[nextIndex].timeInTick;

			float ratio = (animTimeInTick - t1) / static_cast<float>(t2 - t1);
			ratio = glm::clamp(ratio, 0.0f, 1.0f);

			const glm::vec3& beg = nodeAnim.positions[prevIndex].position;
			const glm::vec3& end = nodeAnim.positions[nextIndex].position;

			return glm::mix(beg, end, ratio);
		}

		glm::quat InterpolateRotation(const NodeAnimation& nodeAnim)
		{
			if (nodeAnim.rotations.empty())
			{
				return glm::quat{ 0.0f, 0.0f, 0.0f, 1.0f };
			}

			if (nodeAnim.rotations.size() == 1)
			{
				return nodeAnim.rotations[0].quaternion;
			}

			unsigned int prevIndex = 0;
			for (unsigned int i = 0; i < nodeAnim.rotations.size() - 1; i++)
			{
				if (animTimeInTick < nodeAnim.rotations[i + 1].timeInTick)
				{
					prevIndex = i;
					break;
				}
			}

			unsigned int nextIndex = prevIndex + 1;

			auto t1 = nodeAnim.positions[prevIndex].timeInTick;
			auto t2 = nodeAnim.positions[nextIndex].timeInTick;

			float ratio = (animTimeInTick - t1) / static_cast<float>(t2 - t1);
			ratio = glm::clamp(ratio, 0.0f, 1.0f);

			const glm::quat& beg = nodeAnim.rotations[prevIndex].quaternion;
			const glm::quat& end = nodeAnim.rotations[nextIndex].quaternion;

			glm::quat begNorm = glm::normalize(beg);
			glm::quat endNorm = glm::normalize(end);

			return glm::slerp(begNorm, endNorm, ratio);
		}

		glm::vec3 InterpolateScale(const NodeAnimation& nodeAnim)
		{
			if (nodeAnim.scales.empty())
			{
				return glm::vec3{ 1.0f, 1.0f, 1.0f };
			}

			if (nodeAnim.scales.size() == 1)
			{
				return nodeAnim.scales[0].scale;
			}

			unsigned int prevIndex = 0;
			for (unsigned int i = 0; i < nodeAnim.scales.size() - 1; i++)
			{
				if (animTimeInTick < nodeAnim.scales[i + 1].timeInTick)
				{
					prevIndex = i;
					break;
				}
			}

			unsigned int nextIndex = prevIndex + 1;

			auto t1 = nodeAnim.positions[prevIndex].timeInTick;
			auto t2 = nodeAnim.positions[nextIndex].timeInTick;

			float ratio = (animTimeInTick - t1) / static_cast<float>(t2 - t1);
			ratio = glm::clamp(ratio, 0.0f, 1.0f);

			const glm::vec3& beg = nodeAnim.scales[prevIndex].scale;
			const glm::vec3& end = nodeAnim.scales[nextIndex].scale;

			return glm::mix(beg, end, ratio);
		}

		std::shared_ptr<VertexArrayBase> _vao{ nullptr };
		std::shared_ptr<IndexBufferBase> _ebo{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_ver{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_uv0{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_col{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_nor{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_tan{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_skin{ nullptr };
		std::shared_ptr<VertexBufferBase> _vbo_instanced{ nullptr };

		float animTime{ 0.0f };
		double animTimeInTick{ 0.0 };

	};
}
