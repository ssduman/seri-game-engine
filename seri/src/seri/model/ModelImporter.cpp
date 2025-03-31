#include "Seripch.h"

#include "seri/model/ModelImporter.h"

#include "seri/graphic/Graphic.h"
#include "seri/graphic/MeshG.h"
#include "seri/graphic/MaterialG.h"
#include "seri/camera/ICamera.h"

unsigned int ModelImporter::FlagBuilder()
{
	return
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_FlipUVs |
		aiProcess_CalcTangentSpace |
		aiProcess_OptimizeMeshes |
		0
		;
}

std::vector<std::shared_ptr<MeshG>> ModelImporter::Load(const std::string& modelPath)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(modelPath, FlagBuilder());
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LOGGER(error, "read model path '" << modelPath << "' failed: " << importer.GetErrorString());
		return {};
	}

	_modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";

	std::vector<std::shared_ptr<MeshG>> meshes{};

	glm::mat4 globalTransformation = ConvertMatrix(scene->mRootNode->mTransformation);

	ProcessNode(scene, scene->mRootNode, meshes);

	for (auto& mesh : meshes)
	{
		mesh->transformation = globalTransformation * mesh->transformation;
		mesh->Build();
	}

	LOGGER(info, "read model path '" << modelPath << "' succeeded");
	LOGGER(info, "mesh count: " << meshes.size());

	return meshes;
}

void ModelImporter::ProcessNode(const aiScene* scene, const aiNode* node, std::vector<std::shared_ptr<MeshG>>& meshes)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		std::shared_ptr<MeshG> mesh_ = std::make_shared<MeshG>();
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(scene, node, mesh, mesh_);
		meshes.emplace_back(std::move(mesh_));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(scene, node->mChildren[i], meshes);
	}
}

void ModelImporter::ProcessMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_)
{
	glm::mat4 transformation = ConvertMatrix(node->mTransformation);

	LoadIndices(scene, mesh, mesh_);
	LoadVertices(scene, mesh, mesh_);
	//LoadMaterial(scene, mesh, mesh_);

	mesh_->transformation = transformation;
}

void ModelImporter::LoadIndices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_)
{
	unsigned int offset = static_cast<unsigned int>(mesh_->indices.size());

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.emplace_back(face.mIndices[j] + offset);
		}
	}

	mesh_->AddIndices(std::move(indices));
}

void ModelImporter::LoadVertices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_)
{
	std::vector<glm::vec3> positions;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec2> textureCoords;
	std::vector<glm::vec3> normals;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		if (mesh->HasPositions())
		{
			glm::vec3 position = ConvertVector(mesh->mVertices[i]);
			positions.emplace_back(std::move(position));
		}
		if (mesh->HasVertexColors(0))
		{
			glm::vec4 color = ConvertVector(mesh->mColors[0][i]);
			colors.emplace_back(std::move(color));
		}
		if (mesh->HasTextureCoords(0))
		{
			glm::vec2 textureCoord = ConvertVector(mesh->mTextureCoords[0][i]);
			textureCoords.emplace_back(std::move(textureCoord));
		}
		if (mesh->HasNormals())
		{
			glm::vec3 normal = ConvertVector(mesh->mNormals[i]);
			normals.emplace_back(std::move(normal));
		}
		if (mesh->HasBones())
		{
		}
		if (mesh->HasFaces())
		{
		}
		if (mesh->HasTangentsAndBitangents())
		{
		}
		if (mesh->HasTextureCoordsName(0))
		{
		}
	}

	mesh_->AddVertices(std::move(positions));
	mesh_->AddColors(std::move(colors));
	mesh_->AddUV(std::move(textureCoords));
	mesh_->AddNormals(std::move(normals));
}

void ModelImporter::LoadMaterial(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_)
{
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	LoadTexture(scene, material, aiTextureType_DIFFUSE, mesh_);
	//LoadTexture(scene, material, aiTextureType_SPECULAR, mesh_);
	//LoadTexture(scene, material, aiTextureType_AMBIENT, mesh_);
	//LoadTexture(scene, material, aiTextureType_HEIGHT, mesh_);
	//LoadTexture(scene, material, aiTextureType_NORMALS, mesh_);
	//LoadTexture(scene, material, aiTextureType_SHININESS, mesh_);

	LoadColors(scene, material);
}

void ModelImporter::LoadTexture(const aiScene* scene, const aiMaterial* material, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_)
{
	for (unsigned int i = 0; i < material->GetTextureCount(textureType); i++)
	{
		aiString aiTexturePath;
		if (material->GetTexture(textureType, i, &aiTexturePath) == aiReturn::aiReturn_SUCCESS)
		{
			const std::string texturePath{ aiTexturePath.C_Str() };
			const aiTexture* ai_texture = scene->GetEmbeddedTexture(aiTexturePath.C_Str());
			if (ai_texture)
			{
				LoadEmbeddedTexture(ai_texture, textureType, mesh_);
			}
			else
			{
				LoadFileTexture(texturePath, textureType, mesh_);
			}
		}
		else
		{
			LOGGER(error, "getting " << GetString(textureType) << " texture with index " << i << " failed");
		}
	}
}

void ModelImporter::LoadEmbeddedTexture(const aiTexture* ai_texture, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_)
{
	unsigned int width = ai_texture->mWidth;
	unsigned int height = ai_texture->mHeight;
	unsigned int size = std::max(width, height);
	size = std::max(size, width * height);

	std::vector<Texture> textures;
	Texture texture;
	texture.init(ai_texture->pcData, size);
	textures.emplace_back(std::move(texture));

	mesh_->AddTextures(std::move(textures));
}

void ModelImporter::LoadFileTexture(const std::string& texturePath, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_)
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

	mesh_->AddTextures(std::move(textures));
}

void ModelImporter::LoadColors(const aiScene* scene, const aiMaterial* material)
{
	aiColor4D aiColor;
	if (material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_COLOR_REFLECTIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
	}
	if (material->Get(AI_MATKEY_SHININESS, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor = ConvertVector(aiColor);
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

std::string ModelImporter::GetString(const aiTextureType textureType)
{
	switch (textureType)
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
