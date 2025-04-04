#include "Seripch.h"

#include "seri/model/ModelImporter.h"

#include "seri/graphic/Graphic.h"
#include "seri/graphic/Mesh.h"
#include "seri/graphic/Material.h"
#include "seri/camera/ICamera.h"

#include <filesystem>

unsigned int ModelImporter::FlagBuilder()
{
	return
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		//aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_OptimizeMeshes |
		aiProcess_ValidateDataStructure |
		0
		;
}

std::vector<std::shared_ptr<Mesh>> ModelImporter::Load(const std::string& modelPath)
{
	Assimp::Importer ai_importer;

	const aiScene* ai_scene = ai_importer.ReadFile(modelPath, FlagBuilder());
	if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode)
	{
		LOGGER(error, "read model path '" << modelPath << "' failed: " << ai_importer.GetErrorString());
		return {};
	}

	_modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";

	std::string modelName = std::filesystem::path(modelPath).filename().string();

	std::vector<std::shared_ptr<Mesh>> meshes{};

	glm::mat4 globalTransformation = ConvertMatrix(ai_scene->mRootNode->mTransformation);

	unsigned int triCount = 0;
	for (unsigned int i = 0; i < ai_scene->mNumMeshes; ++i)
	{
		triCount += ai_scene->mMeshes[i]->mNumFaces;
	}

	ProcessNode(ai_scene, ai_scene->mRootNode, meshes);

	LoadAnimations(ai_scene);

	for (auto& mesh : meshes)
	{
		mesh->transformation = globalTransformation * mesh->transformation;
		mesh->Build();
	}

	const char* hasMat = ai_scene->HasMaterials() ? "yes" : "no";
	const char* hasSkel = ai_scene->hasSkeletons() ? "yes" : "no";
	const char* hasAnim = ai_scene->HasAnimations() ? "yes" : "no";

	LOGGER(info,
		"loaded '" << modelName << "', mesh: " << meshes.size() << ", tri: " << triCount <<
		", mat: " << hasMat << ", anim: " << hasAnim << ", skeleton: " << hasSkel
	);

	return meshes;
}

void ModelImporter::ProcessNode(const aiScene* ai_scene, const aiNode* ai_node, std::vector<std::shared_ptr<Mesh>>& meshes)
{
	for (unsigned int i = 0; i < ai_node->mNumMeshes; i++)
	{
		aiMesh* ai_mesh = ai_scene->mMeshes[ai_node->mMeshes[i]];
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		ProcessMesh(ai_scene, ai_node, ai_mesh, mesh);

		if (ai_mesh->HasBones())
		{
			LoadBones(ai_mesh, mesh);
		}

		meshes.emplace_back(std::move(mesh));
	}
	for (unsigned int i = 0; i < ai_node->mNumChildren; i++)
	{
		ProcessNode(ai_scene, ai_node->mChildren[i], meshes);
	}
}

void ModelImporter::ProcessMesh(const aiScene* ai_scene, const aiNode* ai_node, const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
{
	mesh->transformation = ConvertMatrix(ai_node->mTransformation);

	LoadIndices(ai_mesh, mesh);
	LoadVertices(ai_mesh, mesh);
	//LoadMaterial(ai_scene, ai_mesh, mesh);

	LoadBlendShapes(ai_mesh, mesh);
}

void ModelImporter::LoadIndices(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
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

void ModelImporter::LoadVertices(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
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

void ModelImporter::LoadMaterial(const aiScene* ai_scene, const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
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

void ModelImporter::LoadTexture(const aiScene* ai_scene, const aiMaterial* ai_material, const aiTextureType ai_tt, std::shared_ptr<Mesh>& mesh)
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
			LOGGER(error, "getting " << GetString(ai_tt) << " texture with index " << i << " failed");
		}
	}
}

void ModelImporter::LoadEmbeddedTexture(const aiTexture* aiTexture, const aiTextureType ai_tt, std::shared_ptr<Mesh>& mesh)
{
	unsigned int width = aiTexture->mWidth;
	unsigned int height = aiTexture->mHeight;
	unsigned int size = std::max(width, height);
	size = std::max(size, width * height);

	std::vector<Texture> textures;
	Texture texture;
	texture.init(aiTexture->pcData, size);
	textures.emplace_back(std::move(texture));

	mesh->AddTextures(std::move(textures));
}

void ModelImporter::LoadFileTexture(const aiTextureType ai_tt, const std::string& texturePath, std::shared_ptr<Mesh>& mesh)
{
	Texture texture;
	std::vector<Texture> textures;

	if (_texturesLoaded.count(texturePath) == 1)
	{
		texture = _texturesLoaded[texturePath];
	}
	else
	{
		texture.init(_modelDirectory + texturePath);
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

void ModelImporter::LoadBones(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
{
	mesh->bones.resize(mesh->vertices.size());

	for (unsigned int i = 0; i < ai_mesh->mNumBones; i++)
	{
		std::string boneName{ ai_mesh->mBones[i]->mName.data };
		unsigned int boneIndex = 0;
		if (_boneNameToIndexMap.find(boneName) == _boneNameToIndexMap.end())
		{
			boneIndex = static_cast<unsigned int>(_boneNameToIndexMap.size());
			_boneNameToIndexMap[boneName] = boneIndex;
			_boneNameToOffsetMatrixMap[boneName] = ConvertMatrix(ai_mesh->mBones[i]->mOffsetMatrix);
		}
		else
		{
			boneIndex = _boneNameToIndexMap[boneName];
		}

		for (unsigned int j = 0; j < ai_mesh->mBones[i]->mNumWeights; j++)
		{
			const aiVertexWeight& ai_vw = ai_mesh->mBones[i]->mWeights[j];
			mesh->bones[ai_vw.mVertexId].AddBoneData(boneIndex, ai_vw.mWeight);
		}
	}
}

void ModelImporter::LoadAnimations(const aiScene* ai_scene)
{
	if (!ai_scene->HasAnimations())
	{
		LOGGER(info, "no animations found");
		return;
	}

	unsigned int numAnims = ai_scene->mNumAnimations;
	for (unsigned int i = 0; i < numAnims; ++i)
	{
		aiAnimation* ai_animation = ai_scene->mAnimations[i];

		const char* animName = ai_animation->mName.C_Str();
		float durInTick = static_cast<float>(ai_animation->mDuration != 0 ? ai_animation->mDuration : 30.0);
		float tickPerSec = static_cast<float>(ai_animation->mTicksPerSecond);
		float duration = durInTick / tickPerSec;

		for (unsigned int c = 0; c < ai_animation->mNumChannels; c++)
		{
			aiNodeAnim* ai_node_anim = ai_animation->mChannels[c];

			const char* nodeName = ai_node_anim->mNodeName.C_Str();
			if (_boneNameToIndexMap.find(nodeName) == _boneNameToIndexMap.end())
			{
				LOGGER(info, "could not found node: " << nodeName);
				continue;
			}
			int boneIndex = _boneNameToIndexMap[nodeName];

			LoadNodeAnimation(ai_node_anim, boneIndex, tickPerSec);
		}

		LOGGER(info,
			"anim: " << animName << ", duration: " << duration << ", has " <<
			ai_animation->mNumChannels << " skeletal channels, " <<
			ai_animation->mNumMeshChannels << " mesh channels, " <<
			ai_animation->mNumMorphMeshChannels << " morph mesh channels"
		);
	}
}

void ModelImporter::LoadNodeAnimation(const aiNodeAnim* ai_node_anim, int boneIndex, float tickPerSec)
{
	for (unsigned int i = 0; i < ai_node_anim->mNumPositionKeys; i++)
	{
		aiVectorKey ai_vector_key = ai_node_anim->mPositionKeys[i];

		float time = static_cast<float>(ai_vector_key.mTime) / tickPerSec;
		glm::vec3 position = ConvertVector(ai_vector_key.mValue);
	}

	for (unsigned int i = 0; i < ai_node_anim->mNumRotationKeys; i++)
	{
		aiQuatKey ai_quat_key = ai_node_anim->mRotationKeys[i];

		float time = static_cast<float>(ai_quat_key.mTime) / tickPerSec;
		glm::quat rotation = ConvertQuat(ai_quat_key.mValue);
	}

	for (unsigned int i = 0; i < ai_node_anim->mNumScalingKeys; i++)
	{
		aiVectorKey ai_vector_key = ai_node_anim->mScalingKeys[i];

		float time = static_cast<float>(ai_vector_key.mTime) / tickPerSec;
		glm::vec3 scale = ConvertVector(ai_vector_key.mValue);
	}
}

void ModelImporter::LoadBlendShapes(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh)
{
	assert(ai_mesh->mNumAnimMeshes <= 1);

	for (unsigned int i = 0; i < ai_mesh->mNumAnimMeshes; i++)
	{
		aiAnimMesh* ai_anim_mesh = ai_mesh->mAnimMeshes[i];

		const char* blendShapeName = ai_anim_mesh->mName.C_Str();
		float weight = ai_anim_mesh->mWeight;

		LOGGER(info, "blend shape name: '" << blendShapeName << "', weight: " << weight);

		if (ai_anim_mesh->HasPositions())
		{
			for (unsigned int v = 0; v < ai_anim_mesh->mNumVertices; v++)
			{
				glm::vec3 vertex = ConvertVector(ai_anim_mesh->mVertices[v]);
				mesh->blendShapes.emplace_back(vertex);
			}
		}

		LOGGER(info, "loaded " << ai_mesh->mNumAnimMeshes << " blend shape");
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
		ai_quat.x,
		ai_quat.y,
		ai_quat.z,
		ai_quat.w
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
