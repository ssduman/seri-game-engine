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

	//ConvertMatrix(scene->mRootNode->mTransformation, mesh_->transformation);

	ProcessNode(scene, scene->mRootNode, meshes);

	for (auto& mesh : meshes)
	{
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
	glm::mat4 transformation;
	ConvertMatrix(node->mTransformation, transformation);

	LoadIndices(scene, mesh, mesh_);
	LoadVertices(scene, mesh, mesh_);
	LoadMaterial(scene, mesh, mesh_);

	mesh_->SetTransformation(std::move(mesh_->transformation * transformation));
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
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
		glm::vec3 normal;
		if (mesh->HasPositions())
		{
			ConvertVector(mesh->mVertices[i], position);
			positions.emplace_back(std::move(position));
		}
		if (mesh->HasVertexColors(0))
		{
			ConvertVector(mesh->mColors[0][i], color);
			colors.emplace_back(std::move(color));
		}
		if (mesh->HasTextureCoords(0))
		{
			ConvertVector(mesh->mTextureCoords[0][i], textureCoord);
			textureCoords.emplace_back(std::move(textureCoord));
		}
		if (mesh->HasNormals())
		{
			ConvertVector(mesh->mNormals[i], normal);
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
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_COLOR_SPECULAR, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_COLOR_REFLECTIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
	if (material->Get(AI_MATKEY_SHININESS, aiColor) == aiReturn::aiReturn_SUCCESS)
	{
		glm::vec4 glmColor;
		ConvertVector(aiColor, glmColor);
	}
}

void ModelImporter::ConvertVector(const aiVector2D& aiVec, glm::vec2& glmVec)
{
	glmVec.x = aiVec.x;
	glmVec.y = aiVec.y;
}

void ModelImporter::ConvertVector(const aiVector3D& aiVec, glm::vec2& glmVec)
{
	glmVec.x = aiVec.x;
	glmVec.y = aiVec.y;
}

void ModelImporter::ConvertVector(const aiVector3D& aiVec, glm::vec3& glmVec)
{
	glmVec.x = aiVec.x;
	glmVec.y = aiVec.y;
	glmVec.z = aiVec.z;
}

void ModelImporter::ConvertVector(const aiColor4D& aiVec, glm::vec4& glmVec)
{
	glmVec.r = aiVec.r;
	glmVec.g = aiVec.g;
	glmVec.b = aiVec.b;
	glmVec.a = aiVec.a;
}

void ModelImporter::ConvertMatrix(const aiMatrix4x4& aiMat, glm::mat4& glmMat)
{
	glmMat[0].x = aiMat.a1;
	glmMat[0].y = aiMat.a2;
	glmMat[0].z = aiMat.a3;
	glmMat[0].w = aiMat.a4;

	glmMat[1].x = aiMat.b1;
	glmMat[1].y = aiMat.b2;
	glmMat[1].z = aiMat.b3;
	glmMat[1].w = aiMat.b4;

	glmMat[2].x = aiMat.c1;
	glmMat[2].y = aiMat.c2;
	glmMat[2].z = aiMat.c3;
	glmMat[2].w = aiMat.c4;

	glmMat[3].x = aiMat.d1;
	glmMat[3].y = aiMat.d2;
	glmMat[3].z = aiMat.d3;
	glmMat[3].w = aiMat.d4;
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
