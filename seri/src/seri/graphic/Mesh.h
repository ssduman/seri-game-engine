#pragma once

#include "seri/texture/TextureBase.h"
#include "seri/renderer/AuxiliaryStructs.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <map>
#include <vector>
#include <memory>

#define SERI_MAX_BONES 200
#define SERI_MAX_NUM_BONES_PER_VERTEX 4
#define SERI_MAX_INSTANCED_COUNT 256

namespace seri
{
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
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	class Mesh
	{
	public:
		Mesh()
		{
			bones.clear();
		}

		~Mesh() = default;

		std::vector<std::shared_ptr<TextureBase>> textures;
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> indices;
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
		std::vector<glm::vec3> tangents;

		NodeData nodeData{};
		Animation animation{};

		std::map<int, Bone> bones{};
		std::vector<VertexBoneData> bonesForVertices{};
		std::vector<glm::vec3> blendShapes{};

		std::string name{};
		std::string nodeName{};
		std::map<std::string, int> boneNameToIndexMap{};

		glm::mat4 transformation{ 1.0f };

		aux::DrawMode drawMode = aux::DrawMode::triangles;

		unsigned int count = 6;

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

		void AddTextures(std::vector<std::shared_ptr<TextureBase>> texs)
		{
			for (auto& tex : texs)
			{
				textures.emplace_back(std::move(tex));
			}
		}

		void AddIndices(std::vector<unsigned int> ind)
		{
			indices.insert(indices.end(), ind.begin(), ind.end());
		}

		void UpdateAnimation()
		{
			auto time = WindowManagerFactory::Instance()->GetTime();
			auto timeInTicks = time * animation.tickPerSecond;

			animTimeInTick = std::fmod(timeInTicks, animation.durationInTick);
			animTime = static_cast<float>(animTimeInTick) / static_cast<float>(animation.tickPerSecond);
			//LOGGER(info, "time: " << time << ", localAnimationTime: " << localAnimationTime);

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
						LOGGER(error, "bone anim name mismatch: " << nodeAnim.nodeName << ", " << nodeName);
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
						LOGGER(error, "bone name mismatch: " << nodeName << ", " << bones.at(boneIndex).name);
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
			if (_vao != 0)
			{
				LOGGER(warning, "vao already generated");
				return;
			}

			Generate();

			Bind_vao();

			if (vertices.size() > 0)
			{
				Bind_vbo_vertex();

				unsigned int index = aux::toGLenum(aux::Index::position);

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::vbo);
				dataBuffer.size = aux::size(vertices);
				dataBuffer.data = aux::data(vertices);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				aux::Attribute attr;
				attr.index = index;
				attr.size = aux::component(vertices);
				attr.type = aux::toGLenum(aux::Type::float_type);
				attr.normalized = false;
				attr.stride = 0;
				attr.pointer = nullptr;

				glEnableVertexAttribArray(index);
				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
				glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
			}

			if (uv0s.size() > 0)
			{
				Bind_vbo_uv0();

				unsigned int index = aux::toGLenum(aux::Index::texture);

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::vbo);
				dataBuffer.size = aux::size(uv0s);
				dataBuffer.data = aux::data(uv0s);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				aux::Attribute attr;
				attr.index = index;
				attr.size = aux::component(uv0s);
				attr.type = aux::toGLenum(aux::Type::float_type);
				attr.normalized = false;
				attr.stride = 0;
				attr.pointer = nullptr;

				glEnableVertexAttribArray(index);
				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
				glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
			}

			if (colors.size() > 0)
			{
				Bind_vbo_color();

				unsigned int index = aux::toGLenum(aux::Index::color);

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::vbo);
				dataBuffer.size = aux::size(colors);
				dataBuffer.data = aux::data(colors);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				aux::Attribute attr;
				attr.index = index;
				attr.size = aux::component(colors);
				attr.type = aux::toGLenum(aux::Type::float_type);
				attr.normalized = false;
				attr.stride = 0;
				attr.pointer = nullptr;

				glEnableVertexAttribArray(index);
				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
				glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
			}

			if (normals.size() > 0)
			{
				Bind_vbo_normal();

				unsigned int index = aux::toGLenum(aux::Index::normal);

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::vbo);
				dataBuffer.size = aux::size(normals);
				dataBuffer.data = aux::data(normals);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				aux::Attribute attr;
				attr.index = index;
				attr.size = aux::component(normals);
				attr.type = aux::toGLenum(aux::Type::float_type);
				attr.normalized = false;
				attr.stride = 0;
				attr.pointer = nullptr;

				glEnableVertexAttribArray(index);
				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
				glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, attr.pointer);
			}

			if (indices.size() > 0)
			{
				Bind_index();

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::ebo);
				dataBuffer.size = aux::size(indices);
				dataBuffer.data = aux::data(indices);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);
			}

			if (bonesForVertices.size() > 0)
			{
				Bind_vbo_skin();

				unsigned int index_bone_id = aux::toGLenum(aux::Index::skin_bone_id);
				unsigned int index_weight = aux::toGLenum(aux::Index::skin_weight);

				aux::DataBuffer dataBuffer;
				dataBuffer.target = aux::toGLenum(aux::Target::vbo);
				dataBuffer.size = aux::size(bonesForVertices);
				dataBuffer.data = aux::data(bonesForVertices);
				dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

				aux::Attribute attr_bone_id;
				attr_bone_id.index = index_bone_id;
				attr_bone_id.size = 4;
				attr_bone_id.type = aux::toGLenum(aux::Type::int_type);
				attr_bone_id.normalized = false;
				attr_bone_id.stride = sizeof(VertexBoneData);
				attr_bone_id.pointer = nullptr;

				aux::Attribute attr_weight;
				attr_weight.index = index_weight;
				attr_weight.size = 4;
				attr_weight.type = aux::toGLenum(aux::Type::float_type);
				attr_weight.normalized = false;
				attr_weight.stride = sizeof(VertexBoneData);
				attr_weight.pointer = (const void*)(16);

				glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);

				glEnableVertexAttribArray(attr_bone_id.index);
				glVertexAttribIPointer(attr_bone_id.index, attr_bone_id.size, attr_bone_id.type, attr_bone_id.stride, attr_bone_id.pointer);

				glEnableVertexAttribArray(attr_weight.index);
				glVertexAttribPointer(attr_weight.index, attr_weight.size, attr_weight.type, attr_weight.normalized, attr_weight.stride, attr_weight.pointer);
			}

			count = indices.size() > 0 ? static_cast<unsigned int>(indices.size()) : static_cast<unsigned int>(vertices.size());

			Unbind_all();
		}

		void MakeInstanced(const std::vector<glm::mat4>& modelMatrices)
		{
			if (modelMatrices.size() > SERI_MAX_INSTANCED_COUNT)
			{
				LOGGER(error, "model matrices size exceeds max instanced count");
				return;
			}

			if (_vbo_instanced != 0)
			{
				Bind_vao();

				Bind_vbo_instanced();

				void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				memcpy(ptr, modelMatrices.data(), modelMatrices.size() * sizeof(glm::mat4));
				glUnmapBuffer(GL_ARRAY_BUFFER);

				return;
			}

			GenerateInstanced();

			Bind_vao();

			Bind_vbo_instanced();

			unsigned int index = aux::toGLenum(aux::Index::instanced_mat4);

			aux::DataBuffer dataBuffer;
			dataBuffer.target = aux::toGLenum(aux::Target::vbo);
			dataBuffer.size = aux::size(modelMatrices);
			dataBuffer.data = aux::data(modelMatrices);
			dataBuffer.usage = aux::toGLenum(aux::Usage::static_draw);

			aux::Attribute attr;
			attr.index = index;
			attr.size = aux::component(modelMatrices);
			attr.type = aux::toGLenum(aux::Type::float_type);
			attr.normalized = false;
			attr.stride = 0;
			attr.pointer = nullptr;

			glBufferData(dataBuffer.target, dataBuffer.size, dataBuffer.data, dataBuffer.usage);

			for (unsigned int i = 0; i < 4; i++)
			{
				glEnableVertexAttribArray(attr.index + i);
				glVertexAttribPointer(attr.index + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
				glVertexAttribDivisor(attr.index + i, 1);
			}
		}

		bool HasIndex()
		{
			return !indices.empty();
		}

		void Bind()
		{
			Bind_vao();
		}

		void Unbind()
		{
			Unbind_vao();
		}

		static std::unique_ptr<Mesh> tri_2d()
		{
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

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

		static std::unique_ptr<Mesh> quad_2d()
		{
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

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

		static std::unique_ptr<Mesh> quad_3d()
		{
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

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

		static std::unique_ptr<Mesh> cube_3d()
		{
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

			mesh->vertices = {
				  {-1.0f, +1.0f, +1.0f},
				  {-1.0f, -1.0f, +1.0f},
				  {+1.0f, +1.0f, +1.0f},
				  {+1.0f, -1.0f, +1.0f},
				  {-1.0f, +1.0f, -1.0f},
				  {-1.0f, -1.0f, -1.0f},
				  {+1.0f, +1.0f, -1.0f},
				  {+1.0f, -1.0f, -1.0f},
			};

			mesh->uv0s = {
				{0.0f, 0.0f},
				{1.0f, 1.0f},
				{1.0f, 0.0f},
				{0.0f, 1.0f},
				{0.0f, 0.0f},
				{1.0f, 1.0f},
				{1.0f, 0.0f},
				{0.0f, 1.0f},
			};

			mesh->indices = {
				0, 2, 3, 0, 3, 1,
				2, 6, 7, 2, 7, 3,
				6, 4, 5, 6, 5, 7,
				4, 0, 1, 4, 1, 5,
				0, 4, 6, 0, 6, 2,
				1, 5, 7, 1, 7, 3,
			};

			mesh->Build();

			return mesh;
		}

		static std::unique_ptr<Mesh> line_2d(glm::vec2 beg, glm::vec2 end)
		{
			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

			mesh->drawMode = aux::DrawMode::lines;

			float screenW = WindowManagerFactory::Instance()->GetWidthF();
			float screenH = WindowManagerFactory::Instance()->GetHeightF();

			glm::vec4 beg_ = {
				Util::Map(beg.x, 0.0f, screenW, -1.0f, 1.0f),
				Util::Map(beg.y, 0.0f, screenH, -1.0f, 1.0f),
				0.0f,
				0.0f
			};
			glm::vec4 end_ = {
				Util::Map(end.x, 0.0f, screenW, -1.0f, 1.0f),
				Util::Map(end.y, 0.0f, screenH, -1.0f, 1.0f),
				0.0f,
				0.0f
			};

			mesh->vertices = {
				{beg_.x, beg_.y, 0.0f},
				{end_.x, end_.y, 0.0f},
			};

			mesh->Build();

			return mesh;
		}

	private:
		void Generate()
		{
			glGenVertexArrays(1, &_vao);
			glGenBuffers(1, &_vbo_ver);
			glGenBuffers(1, &_vbo_uv0);
			glGenBuffers(1, &_vbo_col);
			glGenBuffers(1, &_vbo_normal);
			glGenBuffers(1, &_vbo_skin);
			glGenBuffers(1, &_ebo);
		}

		void GenerateInstanced()
		{
			glGenBuffers(1, &_vbo_instanced);
		}

		void Bind_vao()
		{
			glBindVertexArray(_vao);
		}

		void Bind_vbo_vertex()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_ver);
		}

		void Bind_vbo_uv0()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_uv0);
		}

		void Bind_vbo_color()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_col);
		}

		void Bind_vbo_normal()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_normal);
		}

		void Bind_vbo_skin()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_skin);
		}

		void Bind_vbo_instanced()
		{
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_instanced);
		}

		void Bind_index()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
		}

		void Unbind_all()
		{
			Unbind_vao();
			Unbind_vbo();
			Unbind_index();
		}

		void Unbind_vao()
		{
			glBindVertexArray(0);
		}

		void Unbind_vbo()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void Unbind_index()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		void Delete_vao()
		{
			glDeleteVertexArrays(1, &_vao);
			glDeleteBuffers(1, &_vbo_ver);
			glDeleteBuffers(1, &_vbo_uv0);
			glDeleteBuffers(1, &_vbo_col);
			glDeleteBuffers(1, &_vbo_normal);
			glDeleteBuffers(1, &_vbo_skin);
			glDeleteBuffers(1, &_vbo_instanced);
			glDeleteBuffers(1, &_ebo);
		}

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

		unsigned int _vao{ 0 };
		unsigned int _vbo_ver{ 0 };
		unsigned int _vbo_uv0{ 0 };
		unsigned int _vbo_col{ 0 };
		unsigned int _vbo_normal{ 0 };
		unsigned int _vbo_skin{ 0 };
		unsigned int _vbo_instanced{ 0 };
		unsigned int _ebo{ 0 };

		float animTime{ 0.0f };
		double animTimeInTick{ 0.0 };

	};
}
