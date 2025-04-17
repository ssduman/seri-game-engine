#include "Seripch.h"

#include "seri/model/ModelImporter.h"

#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Material.h"
#include "seri/graphic/Model.h"
#include "seri/camera/ICamera.h"

#include <string>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace seri
{
	unsigned int ModelImporter::FlagBuilder()
	{
		return
			aiProcess_Triangulate |
			//aiProcess_GenNormals |
			aiProcess_GenSmoothNormals |
			//aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace |
			aiProcess_OptimizeMeshes |
			aiProcess_ValidateDataStructure |
			0
			;
	}

	std::unique_ptr<Model> ModelImporter::Load(const std::string& modelPath)
	{
		Assimp::Importer ai_importer;

		const aiScene* ai_scene = ai_importer.ReadFile(modelPath, FlagBuilder());
		if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
		{
			LOGGER(error, "[model] read model path '" << modelPath << "' failed: " << ai_importer.GetErrorString());
			return nullptr;
		}

		_modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";
		_texturesLoaded.clear();

		std::string modelName = std::filesystem::path(modelPath).filename().string();

		std::unique_ptr<Model> model = std::make_unique<Model>();
		model->meshes.reserve(ai_scene->mNumMeshes);

		glm::mat4 globalTransformation = ConvertMatrix(ai_scene->mRootNode->mTransformation);

		unsigned int triCount = 0;
		for (unsigned int i = 0; i < ai_scene->mNumMeshes; ++i)
		{
			triCount += ai_scene->mMeshes[i]->mNumFaces;
		}

		NodeData nodeData = ProcessNode(ai_scene, ai_scene->mRootNode, model);

		Animation animation = LoadAnimations(ai_scene);

		for (auto& mesh : model->meshes)
		{
			mesh->transformation = globalTransformation * mesh->transformation;
			mesh->nodeData = nodeData;
			mesh->animation = animation;
			mesh->boneNameToIndexMap = _boneNameToIndexMap;
			mesh->Build();
		}

		std::string hasMat = ai_scene->HasMaterials() ? "y" : "n";
		std::string hasSkel = ai_scene->hasSkeletons() ? "y" : "n";
		std::string hasAnim = ai_scene->HasAnimations() ? "y" : "n";

		LOGGER(info,
			"[model] loaded '" << modelName << "', mesh: " << model->meshes.size() << ", tri: " << triCount <<
			", mat: " << hasMat << ", anim: " << hasAnim << ", skeleton: " << hasSkel
		);

		return model;
	}

	NodeData ModelImporter::ProcessNode(const aiScene* ai_scene, const aiNode* ai_node, std::unique_ptr<Model>& model)
	{
		NodeData nodeData;
		nodeData.name = ai_node->mName.C_Str();
		nodeData.transformation = ConvertMatrix(ai_node->mTransformation);
		nodeData.children.reserve(ai_node->mNumChildren);

		for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
		{
			aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];

			std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>();

			mesh->name = ai_mesh->mName.C_Str();
			mesh->nodeName = nodeData.name;
			mesh->transformation = nodeData.transformation;

			ProcessMesh(ai_scene, ai_mesh, mesh);

			LoadBones(ai_mesh, mesh);

			model->meshes.emplace_back(std::move(mesh));
		}

		for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
		{
			nodeData.children.emplace_back(ProcessNode(ai_scene, ai_node->mChildren[i], model));
		}

		return nodeData;
	}

	void ModelImporter::ProcessMesh(const aiScene* ai_scene, const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		LoadIndices(ai_mesh, mesh);
		LoadVertices(ai_mesh, mesh);
		//LoadMaterial(ai_scene, ai_mesh, mesh);

		LoadBlendShapes(ai_mesh, mesh);
	}

	void ModelImporter::LoadIndices(const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		unsigned int offset = static_cast<unsigned int>(mesh->indices.size());

		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < ai_mesh->mNumFaces; i++)
		{
			aiFace aiFace = ai_mesh->mFaces[i];
			for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
			{
				indices.emplace_back(aiFace.mIndices[j] + offset);
			}
		}

		mesh->AddIndices(std::move(indices));
	}

	void ModelImporter::LoadVertices(const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec4> colors;
		std::vector<glm::vec2> uvs0;
		std::vector<glm::vec3> normals;

		for (unsigned int i = 0; i < ai_mesh->mNumVertices; i++)
		{
			if (ai_mesh->HasPositions())
			{
				glm::vec3 vertex = ConvertVector(ai_mesh->mVertices[i]);
				vertices.emplace_back(std::move(vertex));
			}
			if (ai_mesh->HasTextureCoords(0))
			{
				glm::vec2 uv = ConvertVector(ai_mesh->mTextureCoords[0][i]);
				uvs0.emplace_back(std::move(uv));
			}
			if (ai_mesh->HasVertexColors(0))
			{
				glm::vec4 color = ConvertVector(ai_mesh->mColors[0][i]);
				colors.emplace_back(std::move(color));
			}
			if (ai_mesh->HasNormals())
			{
				glm::vec3 normal = ConvertVector(ai_mesh->mNormals[i]);
				normals.emplace_back(std::move(normal));
			}
			if (ai_mesh->HasBones())
			{
			}
			if (ai_mesh->HasFaces())
			{
			}
			if (ai_mesh->HasTangentsAndBitangents())
			{
			}
			if (ai_mesh->HasTextureCoordsName(0))
			{
			}
		}

		mesh->AddVertices(std::move(vertices));
		mesh->AddUV(std::move(uvs0));
		mesh->AddColors(std::move(colors));
		mesh->AddNormals(std::move(normals));
	}

	void ModelImporter::LoadMaterial(const aiScene* ai_scene, const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		aiMaterial* ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];

		LoadTexture(ai_scene, ai_material, aiTextureType_DIFFUSE, mesh);
		//LoadTexture(ai_scene, ai_material, aiTextureType_SPECULAR, mesh);
		//LoadTexture(ai_scene, ai_material, aiTextureType_AMBIENT, mesh);
		//LoadTexture(ai_scene, ai_material, aiTextureType_HEIGHT, mesh);
		//LoadTexture(ai_scene, ai_material, aiTextureType_NORMALS, mesh);
		//LoadTexture(ai_scene, ai_material, aiTextureType_SHININESS, mesh);

		LoadColors(ai_material);
	}

	void ModelImporter::LoadTexture(const aiScene* ai_scene, const aiMaterial* ai_material, const aiTextureType ai_tt, std::unique_ptr<Mesh>& mesh)
	{
		for (unsigned int i = 0; i < ai_material->GetTextureCount(ai_tt); i++)
		{
			aiString ai_tp;
			if (ai_material->GetTexture(ai_tt, i, &ai_tp) == aiReturn::aiReturn_SUCCESS)
			{
				const aiTexture* ai_texture = ai_scene->GetEmbeddedTexture(ai_tp.C_Str());
				if (ai_texture)
				{
					LoadEmbeddedTexture(ai_texture, ai_tt, mesh);
				}
				else
				{
					const std::string texturePath{ ai_tp.C_Str() };
					LoadFileTexture(ai_tt, texturePath, mesh);
				}
			}
			else
			{
				LOGGER(error, "[model] getting " << GetString(ai_tt) << " texture with index " << i << " failed");
			}
		}
	}

	void ModelImporter::LoadEmbeddedTexture(const aiTexture* aiTexture, const aiTextureType ai_tt, std::unique_ptr<Mesh>& mesh)
	{
		unsigned int width = aiTexture->mWidth;
		unsigned int height = aiTexture->mHeight;
		unsigned int size = std::max(width, height);
		size = std::max(size, width * height);

		std::vector<Texture> textures;
		Texture texture;
		texture.Init(aiTexture->pcData, size);
		textures.emplace_back(std::move(texture));

		mesh->AddTextures(std::move(textures));
	}

	void ModelImporter::LoadFileTexture(const aiTextureType ai_tt, const std::string& texturePath, std::unique_ptr<Mesh>& mesh)
	{
		Texture texture;
		std::vector<Texture> textures;

		if (_texturesLoaded.count(texturePath) == 1)
		{
			texture = _texturesLoaded[texturePath];
		}
		else
		{
			texture.Init(_modelDirectory + texturePath);
			_texturesLoaded[texturePath] = texture;
		}

		textures.emplace_back(std::move(texture));

		mesh->AddTextures(std::move(textures));
	}

	void ModelImporter::LoadColors(const aiMaterial* ai_material)
	{
		aiColor4D ai_color;
		if (ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_COLOR_REFLECTIVE, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_COLOR_TRANSPARENT, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
		if (ai_material->Get(AI_MATKEY_SHININESS, ai_color) == aiReturn::aiReturn_SUCCESS)
		{
			glm::vec4 glmColor = ConvertVector(ai_color);
		}
	}

	void ModelImporter::LoadBones(const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		if (!ai_mesh->HasBones())
		{
			return;
		}

		LOGGER(info, "[model] mesh '" << ai_mesh->mName.C_Str() << "' has " << ai_mesh->mNumBones << " bones");

		mesh->bonesForVertices.resize(mesh->vertices.size());

		for (unsigned int i = 0; i < ai_mesh->mNumBones; i++)
		{
			aiBone* ai_bone = ai_mesh->mBones[i];

			std::string boneName = ai_bone->mName.C_Str();
			glm::mat4 boneOffsetMatrix = ConvertMatrix(ai_bone->mOffsetMatrix);

			unsigned int boneIndex = -1;
			if (_boneNameToIndexMap.find(boneName) == _boneNameToIndexMap.end())
			{
				boneIndex = static_cast<unsigned int>(_boneNameToIndexMap.size());

				_boneNameToIndexMap[boneName] = boneIndex;
			}
			else
			{
				boneIndex = _boneNameToIndexMap[boneName];
			}

			if (mesh->bones.find(boneIndex) == mesh->bones.end())
			{
				mesh->bones[boneIndex] = Bone{ boneIndex, boneName, boneOffsetMatrix };
			}

			//LOGGER(info, "[model] mesh: " << mesh->name << ", bone index: " << boneIndex << ", bone name: " << boneName);

			for (unsigned int j = 0; j < ai_bone->mNumWeights; j++)
			{
				const aiVertexWeight& ai_vw = ai_bone->mWeights[j];
				mesh->bonesForVertices[ai_vw.mVertexId].AddBoneData(boneIndex, ai_vw.mWeight);
			}
		}
	}

	Animation ModelImporter::LoadAnimations(const aiScene* ai_scene)
	{
		if (!ai_scene->HasAnimations())
		{
			LOGGER(info, "[model] no animations found");
			return {};
		}

		Animation animation{};

		unsigned int numAnims = ai_scene->mNumAnimations;

		for (unsigned int i = 0; i < numAnims; ++i)
		{
			aiAnimation* ai_animation = ai_scene->mAnimations[i];

			std::string animName = ai_animation->mName.C_Str();
			auto durInTick = ai_animation->mDuration != 0 ? ai_animation->mDuration : 30.0;
			auto tickPerSec = ai_animation->mTicksPerSecond;
			float duration = static_cast<float>(durInTick) / static_cast<float>(tickPerSec);

			animation.name = animName;
			animation.durationInTick = durInTick;
			animation.tickPerSecond = tickPerSec;

			for (unsigned int c = 0; c < ai_animation->mNumChannels; c++)
			{
				aiNodeAnim* ai_node_anim = ai_animation->mChannels[c];

				std::string nodeName = ai_node_anim->mNodeName.C_Str();

				if (animation.nodeAnimations.find(nodeName) != animation.nodeAnimations.end())
				{
					LOGGER(error, "[model] duplicate node animation found: " << nodeName);
					continue;
				}

				animation.nodeAnimations[nodeName] = LoadNodeAnimation(ai_node_anim);
			}

			LOGGER(info,
				"[model] anim: " << animName << ", duration: " << duration << ", has " <<
				ai_animation->mNumChannels << " skeletal, " <<
				ai_animation->mNumMeshChannels << " mesh channels, " <<
				ai_animation->mNumMorphMeshChannels << " morph mesh"
			);
		}

		return animation;
	}

	NodeAnimation ModelImporter::LoadNodeAnimation(const aiNodeAnim* ai_node_anim)
	{
		std::string nodeName = ai_node_anim->mNodeName.C_Str();

		NodeAnimation anim{};
		anim.nodeName = nodeName;

		for (unsigned int i = 0; i < ai_node_anim->mNumPositionKeys; i++)
		{
			aiVectorKey ai_vector_key = ai_node_anim->mPositionKeys[i];

			double timeInTick = ai_vector_key.mTime;
			glm::vec3 position = ConvertVector(ai_vector_key.mValue);

			anim.positions.emplace_back(AnimPositionKey{ timeInTick, position });
		}

		for (unsigned int i = 0; i < ai_node_anim->mNumRotationKeys; i++)
		{
			aiQuatKey ai_quat_key = ai_node_anim->mRotationKeys[i];

			double timeInTick = ai_quat_key.mTime;
			glm::quat rotation = ConvertQuat(ai_quat_key.mValue);

			anim.rotations.emplace_back(AnimRotationKey{ timeInTick, rotation });
		}

		for (unsigned int i = 0; i < ai_node_anim->mNumScalingKeys; i++)
		{
			aiVectorKey ai_vector_key = ai_node_anim->mScalingKeys[i];

			double timeInTick = ai_vector_key.mTime;
			glm::vec3 scale = ConvertVector(ai_vector_key.mValue);

			anim.scales.emplace_back(AnimScaleKey{ timeInTick, scale });
		}

		std::sort(anim.positions.begin(), anim.positions.end(), [](const AnimPositionKey& a, const AnimPositionKey& b)
			{
				return a.timeInTick < b.timeInTick;
			}
		);

		std::sort(anim.rotations.begin(), anim.rotations.end(), [](const AnimRotationKey& a, const AnimRotationKey& b)
			{
				return a.timeInTick < b.timeInTick;
			}
		);

		std::sort(anim.scales.begin(), anim.scales.end(), [](const AnimScaleKey& a, const AnimScaleKey& b)
			{
				return a.timeInTick < b.timeInTick;
			}
		);

		return anim;
	}

	void ModelImporter::LoadBlendShapes(const aiMesh* ai_mesh, std::unique_ptr<Mesh>& mesh)
	{
		assert(ai_mesh->mNumAnimMeshes <= 1);

		for (unsigned int i = 0; i < ai_mesh->mNumAnimMeshes; i++)
		{
			aiAnimMesh* ai_anim_mesh = ai_mesh->mAnimMeshes[i];

			std::string blendShapeName = ai_anim_mesh->mName.C_Str();
			float weight = ai_anim_mesh->mWeight;

			LOGGER(info, "[model] blend shape name: '" << blendShapeName << "', weight: " << weight);

			if (ai_anim_mesh->HasPositions())
			{
				for (unsigned int v = 0; v < ai_anim_mesh->mNumVertices; v++)
				{
					glm::vec3 vertex = ConvertVector(ai_anim_mesh->mVertices[v]);
					mesh->blendShapes.emplace_back(vertex);
				}
			}

			LOGGER(info, "[model] loaded " << ai_mesh->mNumAnimMeshes << " blend shape");
		}
	}

	glm::vec2 ModelImporter::ConvertVector(const aiVector2D& ai_vec)
	{
		return {
			ai_vec.x,
			ai_vec.y
		};
	}

	glm::vec3 ModelImporter::ConvertVector(const aiVector3D& ai_vec)
	{
		return {
			ai_vec.x,
			ai_vec.y,
			ai_vec.z
		};
	}

	glm::vec4 ModelImporter::ConvertVector(const aiColor4D& ai_vec)
	{
		return {
			ai_vec.r,
			ai_vec.g,
			ai_vec.b,
			ai_vec.a
		};
	}

	glm::quat ModelImporter::ConvertQuat(const aiQuaternion& ai_quat)
	{
		return {
			ai_quat.w,
			ai_quat.x,
			ai_quat.y,
			ai_quat.z
		};
	}

	glm::mat4 ModelImporter::ConvertMatrix(const aiMatrix4x4& ai_mat)
	{
		return {
			ai_mat.a1, ai_mat.b1, ai_mat.c1, ai_mat.d1,
			ai_mat.a2, ai_mat.b2, ai_mat.c2, ai_mat.d2,
			ai_mat.a3, ai_mat.b3, ai_mat.c3, ai_mat.d3,
			ai_mat.a4, ai_mat.b4, ai_mat.c4, ai_mat.d4
		};
	}

	std::string ModelImporter::GetString(const aiTextureType ai_tt)
	{
		switch (ai_tt)
		{
			case aiTextureType_NONE: return "none";
			case aiTextureType_DIFFUSE: return "diffuse";
			case aiTextureType_SPECULAR: return "specular";
			case aiTextureType_AMBIENT: return "ambient";
			case aiTextureType_EMISSIVE: return "emissive";
			case aiTextureType_HEIGHT: return "height";
			case aiTextureType_NORMALS: return "normals";
			case aiTextureType_SHININESS: return "shininess";
			case aiTextureType_OPACITY: return "opacity";
			case aiTextureType_DISPLACEMENT: return "displacement";
			case aiTextureType_LIGHTMAP: return "lightmap";
			case aiTextureType_REFLECTION: return "reflection";
			default: return "unknown";
		}
	}
}
