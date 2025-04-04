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

	void ProcessNode(const aiScene* ai_scene, const aiNode* ai_node, std::vector<std::shared_ptr<Mesh>>& meshes);

	void ProcessMesh(const aiScene* ai_scene, const aiNode* ai_node, const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	void LoadIndices(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	void LoadVertices(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	void LoadMaterial(const aiScene* ai_scene, const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	void LoadTexture(const aiScene* ai_scene, const aiMaterial* ai_material, const aiTextureType ai_tt, std::shared_ptr<Mesh>& mesh);

	void LoadEmbeddedTexture(const aiTexture* ai_texture, const aiTextureType ai_tt, std::shared_ptr<Mesh>& mesh);

	void LoadFileTexture(const aiTextureType ai_tt, const std::string& texturePath, std::shared_ptr<Mesh>& mesh);

	void LoadColors(const aiMaterial* ai_material);

	void LoadBones(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	void LoadAnimations(const aiScene* ai_scene);

	void LoadNodeAnimation(const aiNodeAnim* ai_node_anim, int boneIndex, float tickPerSec);

	void LoadBlendShapes(const aiMesh* ai_mesh, std::shared_ptr<Mesh>& mesh);

	static glm::vec2 ConvertVector(const aiVector2D& ai_vec);

	static glm::vec3 ConvertVector(const aiVector3D& ai_vec);

	static glm::vec4 ConvertVector(const aiColor4D& ai_vec);

	static glm::quat ConvertQuat(const aiQuaternion& ai_quat);

	static glm::mat4 ConvertMatrix(const aiMatrix4x4& ai_mat);

	static std::string GetString(const aiTextureType ai_tt);

	std::map<std::string, Texture> _texturesLoaded;
	std::map<std::string, int> _boneNameToIndexMap;
	std::map<std::string, glm::mat4> _boneNameToOffsetMatrixMap;
	std::string _modelDirectory;

};
