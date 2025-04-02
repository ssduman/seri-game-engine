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

class Mesh;

class ModelImporter
{
public:
	ModelImporter()
	{
		LOGGER(info, "model importer init succeeded");
	};

	~ModelImporter() = default;

	std::vector<std::shared_ptr<Mesh>> Load(const std::string& modelPath);

private:
	unsigned int FlagBuilder();

	void ProcessNode(const aiScene* aiScene, const aiNode* aiNode, std::vector<std::shared_ptr<Mesh>>& meshes);

	void ProcessMesh(const aiScene* aiScene, const aiNode* aiNode, const aiMesh* mesh, std::shared_ptr<Mesh>& mesh_);

	void LoadIndices(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh_);

	void LoadVertices(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh_);

	void LoadMaterial(const aiScene* aiScene, const aiMesh* aiMesh, std::shared_ptr<Mesh>& mesh_);

	void LoadTexture(const aiScene* aiScene, const aiMaterial* aiMaterial, const aiTextureType aiTextureType, std::shared_ptr<Mesh>& mesh_);

	void LoadEmbeddedTexture(const aiTexture* aiTexture, const aiTextureType aiTextureType, std::shared_ptr<Mesh>& mesh_);

	void LoadFileTexture(const aiTextureType aiTextureType, const std::string& texturePath, std::shared_ptr<Mesh>& mesh_);

	void LoadColors(const aiScene* aiScene, const aiMaterial* aiMaterial);

	void LoadAnimations(const aiScene* aiScene);

	static glm::vec2 ConvertVector(const aiVector2D& aiVec);

	static glm::vec3 ConvertVector(const aiVector3D& aiVec);

	static glm::vec4 ConvertVector(const aiColor4D& aiVec);

	static glm::mat4 ConvertMatrix(const aiMatrix4x4& aiMat);

	static std::string GetString(const aiTextureType aiTextureType);

	std::map<std::string, Texture> _texturesLoaded;
	std::string _modelDirectory;

};
