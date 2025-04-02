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
	Assimp::Importer aiImporter;

	const aiScene* aiScene = aiImporter.ReadFile(modelPath, FlagBuilder());
	if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
	{
		LOGGER(error, "read model path '" << modelPath << "' failed: " << aiImporter.GetErrorString());
		return {};
	}

	_modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";

	std::string modelName = std::filesystem::path(modelPath).filename().string();

	std::vector<std::shared_ptr<Mesh>> meshes{};

	glm::mat4 globalTransformation = ConvertMatrix(aiScene->mRootNode->mTransformation);

	unsigned int triCount = 0;
	for (unsigned int i = 0; i < aiScene->mNumMeshes; ++i)
	{
		unsigned int numFaces = aiScene->mMeshes[i]->mNumFaces;

		triCount += numFaces;
	}

	ProcessNode(aiScene, aiScene->mRootNode, meshes);

	LoadAnimations(aiScene);

	for (auto& mesh : meshes)
	{
		mesh->transformation = globalTransformation * mesh->transformation;
		mesh->Build();
	}

	LOGGER(info, "loaded '" << modelName << "', " << "mesh count: " << meshes.size() << ", tri count: " << triCount);

	return meshes;
}

void ModelImporter::ProcessNode(const aiScene* aiScene, const aiNode* aiNode, std::vector<std::shared_ptr<Mesh>>& meshes)
{
	for (unsigned int i = 0; i < aiNode->mNumMeshes; i++)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		aiMesh* aiMesh = aiScene->mMeshes[aiNode->mMeshes[i]];
		ProcessMesh(aiScene, aiNode, aiMesh, mesh);

		if (aiMesh->HasBones())
		{
			LoadBones(aiMesh, mesh);
		}

		meshes.emplace_back(std::move(mesh));
	}
	for (unsigned int i = 0; i < aiNode->mNumChildren; i++)
	{
		ProcessNode(aiScene, aiNode->mChildren[i], meshes);
	}
}

void ModelImporter::ProcessMesh(const aiScene* aiScene, const aiNode* aiNode, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh)
{
	glm::mat4 transformation = ConvertMatrix(aiNode->mTransformation);

	LoadIndices(aiScene, aiMesh, mesh);
	LoadVertices(aiScene, aiMesh, mesh);
	//LoadMaterial(aiScene, aiMesh, mesh);

	mesh->transformation = transformation;
}

void ModelImporter::LoadIndices(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh)
{
	unsigned int offset = static_cast<unsigned int>(mesh->indices.size());

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
	{
		aiFace aiFace = aiMesh->mFaces[i];
		for (unsigned int j = 0; j < aiFace.mNumIndices; j++)
		{
			indices.emplace_back(aiFace.mIndices[j] + offset);
		}
	}

	mesh->AddIndices(std::move(indices));
}

void ModelImporter::LoadVertices(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals;
	for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
	{
		if (aiMesh->HasPositions())
		{
			glm::vec3 position = ConvertVector(aiMesh->mVertices[i]);
			positions.emplace_back(std::move(position));
		}
		if (aiMesh->HasVertexColors(0))
		{
			glm::vec4 color = ConvertVector(aiMesh->mColors[0][i]);
			colors.emplace_back(std::move(color));
		}
		if (aiMesh->HasTextureCoords(0))
		{
			glm::vec2 textureCoord = ConvertVector(aiMesh->mTextureCoords[0][i]);
			textureCoords.emplace_back(std::move(textureCoord));
		}
		if (aiMesh->HasNormals())
		{
			glm::vec3 normal = ConvertVector(aiMesh->mNormals[i]);
			normals.emplace_back(std::move(normal));
		}
		if (aiMesh->HasBones())
		{
		}
		if (aiMesh->HasFaces())
		{
		}
		if (aiMesh->HasTangentsAndBitangents())
		{
		}
		if (aiMesh->HasTextureCoordsName(0))
		{
		}
	}

	mesh->AddVertices(std::move(positions));
	mesh->AddColors(std::move(colors));
	mesh->AddUV(std::move(textureCoords));
	mesh->AddNormals(std::move(normals));
}

void ModelImporter::LoadMaterial(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh)
{
	aiMaterial* aiMaterial = aiScene->mMaterials[aiMesh->mMaterialIndex];

	LoadTexture(aiScene, aiMaterial, aiTextureType_DIFFUSE, mesh);
	//LoadTexture(aiScene, aiMaterial, aiTextureType_SPECULAR, mesh);
	//LoadTexture(aiScene, aiMaterial, aiTextureType_AMBIENT, mesh);
	//LoadTexture(aiScene, aiMaterial, aiTextureType_HEIGHT, mesh);
	//LoadTexture(aiScene, aiMaterial, aiTextureType_NORMALS, mesh);
	//LoadTexture(aiScene, aiMaterial, aiTextureType_SHININESS, mesh);

	LoadColors(aiScene, aiMaterial);
}

void ModelImporter::LoadTexture(const aiScene* aiScene, const aiMaterial* aiMaterial, const aiTextureType aiTextureType, std::shared_ptr<Mesh>& mesh)
{
	for (unsigned int i = 0; i < aiMaterial->GetTextureCount(aiTextureType); i++)
	{
		aiString aiTexturePath;
		if (aiMaterial->GetTexture(aiTextureType, i, &aiTexturePath) == aiReturn::aiReturn_SUCCESS)
		{
			const std::string texturePath{ aiTexturePath.C_Str() };
			const aiTexture* aiTexture = aiScene->GetEmbeddedTexture(aiTexturePath.C_Str());
			if (aiTexture)
			{
				LoadEmbeddedTexture(aiTexture, aiTextureType, mesh);
			}
			else
			{
				LoadFileTexture(aiTextureType, texturePath, mesh);
			}
		}
		else
		{
			LOGGER(error, "getting " << GetString(aiTextureType) << " texture with index " << i << " failed");
		}
	}
}

void ModelImporter::LoadEmbeddedTexture(const aiTexture* aiTexture, const aiTextureType aiTextureType, std::shared_ptr<Mesh>& mesh)
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

void ModelImporter::LoadFileTexture(const aiTextureType aiTextureType, const std::string& texturePath, std::shared_ptr<Mesh>& mesh)
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

void ModelImporter::LoadColors(const aiScene* aiScene, const aiMaterial* aiMaterial)
{
	aiColor4D aiColor;
	if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_COLOR_REFLECTIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (aiMaterial->Get(AI_MATKEY_SHININESS, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
}

void ModelImporter::LoadBones(const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh)
{
	mesh->bones.resize(mesh->vertices.size());

	for (unsigned int i = 0; i < aiMesh->mNumBones; i++)
	{
		std::string boneName{ aiMesh->mBones[i]->mName.data };
		unsigned int boneIndex = 0;
		if (_internalBoneNameToIndexMap.find(boneName) == _internalBoneNameToIndexMap.end())
		{
			boneIndex = static_cast<unsigned int>(_internalBoneNameToIndexMap.size());
			_internalBoneNameToIndexMap[boneName] = boneIndex;
		}
		else
		{
			boneIndex = _internalBoneNameToIndexMap[boneName];
		}

		for (unsigned int j = 0; j < aiMesh->mBones[i]->mNumWeights; j++)
		{
			const aiVertexWeight& aiVW = aiMesh->mBones[i]->mWeights[j];
			unsigned int vertexId = aiMesh->mBones[i]->mWeights[j].mVertexId;
			mesh->bones[vertexId].AddBoneData(boneIndex, aiVW.mWeight);
		}
	}
}

void ModelImporter::LoadAnimations(const aiScene* aiScene)
{
	unsigned int numAnims = aiScene->mNumAnimations;
	for (unsigned int i = 0; i < numAnims; ++i)
	{
		aiAnimation* aiAnimation = aiScene->mAnimations[i];

		const char* animName = aiAnimation->mName.C_Str();

		LOGGER(info,
			"anim: " << animName << ", duration: " << aiAnimation->mDuration << ", has " <<
			aiAnimation->mNumChannels << " skeletal channels, " <<
			aiAnimation->mNumMeshChannels << " mesh channels, " <<
			aiAnimation->mNumMorphMeshChannels << " morph mesh channels"
		);
	}
}

glm::vec2 ModelImporter::ConvertVector(const aiVector2D& aiVec)
{
	return {
		aiVec.x,
		aiVec.y
	};
}

glm::vec3 ModelImporter::ConvertVector(const aiVector3D& aiVec)
{
	return {
		aiVec.x,
		aiVec.y,
		aiVec.z
	};
}

glm::vec4 ModelImporter::ConvertVector(const aiColor4D& aiVec)
{
	return {
		aiVec.r,
		aiVec.g,
		aiVec.b,
		aiVec.a
	};
}

glm::mat4 ModelImporter::ConvertMatrix(const aiMatrix4x4& aiMat)
{
	return {
		aiMat.a1, aiMat.b1, aiMat.c1, aiMat.d1,
		aiMat.a2, aiMat.b2, aiMat.c2, aiMat.d2,
		aiMat.a3, aiMat.b3, aiMat.c3, aiMat.d3,
		aiMat.a4, aiMat.b4, aiMat.c4, aiMat.d4
	};
}

std::string ModelImporter::GetString(const aiTextureType aiTextureType)
{
	switch (aiTextureType)
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
