#pragma once

#include "seri/model/Mesh.h"
#include "seri/model/Material.h"
#include "seri/core/Object.h"
#include "seri/logging/Logger.h"
#include "seri/math/Transform.h"
#include "seri/shader/Shader.h"
#include "seri/shader/ShaderManager.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <map>
#include <memory>
#include <vector>

class Model : public Object
{
public:
	Model(std::shared_ptr<ICamera> camera) : _camera(camera)
	{
		LOGGER(info, "model init succeeded");
	};

	~Model() override = default;

	void init() override
	{
		_shaderManager.initMVP(_camera);
	};

	void update() override
	{
		if (_camera)
		{
			_shaderManager.setView(_camera->getView());
		}
	};

	void render() override {};

	void display() override
	{
		update();
		render();
		for (auto& mesh : _meshes)
		{
			mesh.display();
		}
	}

	void load(const std::string& modelPath)
	{
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(modelPath, flagBuilder());
		if (!scene)
		{
			LOGGER(error, "read model path '" << modelPath << "' failed: " << importer.GetErrorString());
			return;
		}

		convertMatrix(scene->mRootNode->mTransformation, _globalTransformation);
		_shaderManager.setModel(_globalTransformation);

		_modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";

		LOGGER(info, "read model path '" << modelPath << "' succeeded");
		LOGGER(info, "model directory: " << _modelDirectory);

		processNode(scene, scene->mRootNode);

		_texturesLoaded.clear();
	}

	Shader& getShader()
	{
		return _shader;
	}

	Transform& getTransform()
	{
		return _transform;
	}

	ShaderManager& getShaderManager()
	{
		return _shaderManager;
	}

private:
	unsigned int flagBuilder()
	{
		return aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_FlipUVs |
			aiProcess_CalcTangentSpace;
	}

	void processNode(const aiScene* scene, const aiNode* node)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(scene, node, mesh);
		}
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(scene, node->mChildren[i]);
		}
	}

	void processMesh(const aiScene* scene, const aiNode* node, const aiMesh* mesh)
	{
		glm::mat4 transformation;
		convertMatrix(node->mTransformation, transformation);

		Mesh mesh_{ _shader };

		loadIndices(scene, mesh, mesh_);
		loadVertices(scene, mesh, mesh_);
		loadMaterial(scene, mesh, mesh_);

		mesh_.init();
		mesh_.setTransformation(std::move(_globalTransformation * transformation));

		_meshes.emplace_back(std::move(mesh_));
	}

	void loadIndices(const aiScene* scene, const aiMesh* mesh, Mesh& mesh_)
	{
		std::vector<unsigned int> indices;
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.emplace_back(face.mIndices[j]);
			}
		}
		mesh_.addIndices(std::move(indices));
	}

	void loadVertices(const aiScene* scene, const aiMesh* mesh, Mesh& mesh_)
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
				convertVector(mesh->mVertices[i], position);
				positions.emplace_back(std::move(position));
			}
			if (mesh->HasVertexColors(0))
			{
				convertVector(mesh->mColors[0][i], color);
				colors.emplace_back(std::move(color));
			}
			if (mesh->HasTextureCoords(0))
			{
				convertVector(mesh->mTextureCoords[0][i], textureCoord);
				textureCoords.emplace_back(std::move(textureCoord));
			}
			if (mesh->HasNormals())
			{
				convertVector(mesh->mNormals[i], normal);
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
		mesh_.addPositions(std::move(positions));
		mesh_.addColors(std::move(colors));
		mesh_.addTextureCoords(std::move(textureCoords));
		mesh_.addNormals(std::move(normals));
	}

	void loadMaterial(const aiScene* scene, const aiMesh* mesh, Mesh& mesh_)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		Texture::setTextureFlip(true);

		loadTexture(scene, material, aiTextureType_DIFFUSE, mesh_);
		//loadTexture(scene, material, aiTextureType_SPECULAR, mesh_);
		//loadTexture(scene, material, aiTextureType_AMBIENT, mesh_);
		//loadTexture(scene, material, aiTextureType_HEIGHT, mesh_);
		//loadTexture(scene, material, aiTextureType_NORMALS, mesh_);
		//loadTexture(scene, material, aiTextureType_SHININESS, mesh_);

		Material material_;
		loadColors(scene, material, material_);
	}

	void loadTexture(const aiScene* scene, const aiMaterial* material, const aiTextureType textureType, Mesh& mesh_)
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
					loadEmbeddedTexture(ai_texture, textureType, mesh_);
				}
				else
				{
					loadFileTexture(texturePath, textureType, mesh_);
				}
			}
			else
			{
				LOGGER(error, "getting " << getString(textureType) << " texture with index " << i << " failed");
			}
		}
	}

	void loadEmbeddedTexture(const aiTexture* ai_texture, const aiTextureType textureType, Mesh& mesh_)
	{
		unsigned int width = ai_texture->mWidth;
		unsigned int height = ai_texture->mHeight;
		unsigned int size = std::max(width, height);
		size = std::max(size, width * height);

		std::vector<Texture> textures;
		Texture texture;
		texture.init(ai_texture->pcData, size);
		textures.emplace_back(std::move(texture));
		mesh_.addTextures(std::move(textures));
	}

	void loadFileTexture(const std::string& texturePath, const aiTextureType textureType, Mesh& mesh_)
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
		mesh_.addTextures(std::move(textures));
	}

	void loadColors(const aiScene* scene, const aiMaterial* material, Material& material_)
	{
		aiColor4D aiColor;
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getDiffuse());
		}
		if (material->Get(AI_MATKEY_COLOR_AMBIENT, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getAmbient());
		}
		if (material->Get(AI_MATKEY_COLOR_SPECULAR, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getSpecular());
		}
		if (material->Get(AI_MATKEY_COLOR_EMISSIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getEmissive());
		}
		if (material->Get(AI_MATKEY_COLOR_REFLECTIVE, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getReflective());
		}
		if (material->Get(AI_MATKEY_COLOR_TRANSPARENT, aiColor) == aiReturn::aiReturn_SUCCESS)
		{
			convertVector(aiColor, material_.getTransparent());
		}
		if (material->Get(AI_MATKEY_SHININESS, material_.getShininess()) == aiReturn::aiReturn_SUCCESS)
		{
		}
	}

	static void convertVector(const aiVector2D& aiVec, glm::vec2& glmVec)
	{
		glmVec.x = aiVec.x;
		glmVec.y = aiVec.y;
	}

	static void convertVector(const aiVector3D& aiVec, glm::vec2& glmVec)
	{
		glmVec.x = aiVec.x;
		glmVec.y = aiVec.y;
	}

	static void convertVector(const aiVector3D& aiVec, glm::vec3& glmVec)
	{
		glmVec.x = aiVec.x;
		glmVec.y = aiVec.y;
		glmVec.z = aiVec.z;
	}

	static void convertVector(const aiColor4D& aiVec, glm::vec4& glmVec)
	{
		glmVec.r = aiVec.r;
		glmVec.g = aiVec.g;
		glmVec.b = aiVec.b;
		glmVec.a = aiVec.a;
	}

	static void convertMatrix(const aiMatrix4x4& aiMat, glm::mat4& glmMat)
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

	static std::string getString(const aiTextureType textureType)
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

	std::shared_ptr<ICamera> _camera;
	Shader _shader;
	ShaderManager _shaderManager{ _shader };

	Transform _transform;
	std::vector<Mesh> _meshes;
	std::map<std::string, Texture> _texturesLoaded;
	std::string _modelDirectory;
	glm::mat4 _globalTransformation{};

};
