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

	std::shared_ptr<MeshG> load(const std::string& modelPath);

private:
	void processNode(const aiScene* scene, const aiNode* node, std::shared_ptr<MeshG> mesh_);

	void processMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void loadIndices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void loadVertices(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void loadMaterial(const aiScene* scene, const aiMesh* mesh, std::shared_ptr<MeshG> mesh_);

	void loadTexture(const aiScene* scene, const aiMaterial* material, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void loadEmbeddedTexture(const aiTexture* ai_texture, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void loadFileTexture(const std::string& texturePath, const aiTextureType textureType, std::shared_ptr<MeshG> mesh_);

	void loadColors(const aiScene* scene, const aiMaterial* material);
	
	unsigned int flagBuilder();

	static void convertVector(const aiVector2D& aiVec, glm::vec2& glmVec);

	static void convertVector(const aiVector3D& aiVec, glm::vec2& glmVec);

	static void convertVector(const aiVector3D& aiVec, glm::vec3& glmVec);

	static void convertVector(const aiColor4D& aiVec, glm::vec4& glmVec);

	static void convertMatrix(const aiMatrix4x4& aiMat, glm::mat4& glmMat);

	static std::string getString(const aiTextureType textureType);

	std::map<std::string, Texture> _texturesLoaded;
	std::string _modelDirectory;

};
