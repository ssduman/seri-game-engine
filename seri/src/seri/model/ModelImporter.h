#pragma once

#include "seri/logging/Logger.h"
#include "seri/texture/Texture.h"

#include <glm/glm.hpp>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <map>
#include <memory>
#include <vector>
#include <string>

class MeshG;

class ModelImporter
{
public:
	ModelImporter()
	{
		LOGGER(info, "model importer init succeeded");
	};

	~ModelImporter() = default;

	std::vector<std::shared_ptr<MeshG>> Load(const std::string& modelPath);

private:
	unsigned int FlagBuilder();

	void ProcessNode(const aiScene* scene, const aiNode* node, std::vector<std::shared_ptr<MeshG>>& meshes);

	void ProcessMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void LoadIndices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void LoadVertices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void LoadMaterial(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void LoadTexture(const aiScene* scene, const aiMaterial* material, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void LoadEmbeddedTexture(const aiTexture* ai_texture, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void LoadFileTexture(const std::string& texturePath, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void LoadColors(const aiScene* scene, const aiMaterial* material);

	static glm::vec2 ConvertVector(const aiVector2D& aiVec);

	static glm::vec3 ConvertVector(const aiVector3D& aiVec);

	static glm::vec4 ConvertVector(const aiColor4D& aiVec);

	static glm::mat4 ConvertMatrix(const aiMatrix4x4& aiMat);

	static std::string GetString(const aiTextureType textureType);

	std::map<std::string, Texture> _texturesLoaded;
	std::string _modelDirectory;

};
