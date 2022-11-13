#pragma once

#include "Mesh.h"
#include "Entity.h"
#include "Logger.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <memory>
#include <vector>

class Model : public Entity {
public:
    Model(ICamera* camera) : Entity(camera) {
        LOGGER(info, "model init succeeded");
    };

    ~Model() override = default;

    void init() override {}

    void render() override {
        for (auto& mesh : _meshes) {
            mesh.display();
        }
    }

    void load(const std::string& modelPath) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(modelPath, flagBuilder());
        if (!scene) {
            LOGGER(error, "read model path '" << modelPath << "' failed: " << importer.GetErrorString());
            return;
        }

        _modelDirectory = modelPath.substr(0, modelPath.find_last_of("/")) + "/";

        LOGGER(info, "read model path '" << modelPath << "' succeeded");
        LOGGER(info, "model directory: " << _modelDirectory);
        LOGGER(info, "model mNumMeshes: " << scene->mNumMeshes);
        LOGGER(info, "model mNumMaterials: " << scene->mNumMaterials);
        LOGGER(info, "model mNumAnimations: " << scene->mNumAnimations);
        LOGGER(info, "model mNumTextures: " << scene->mNumTextures);
        LOGGER(info, "model mNumLights: " << scene->mNumLights);
        LOGGER(info, "model mNumCameras: " << scene->mNumCameras);
        LOGGER(info, "model mNumSkeletons: " << scene->mNumSkeletons);

        processNode(scene->mRootNode, scene);
    }

private:
    unsigned int flagBuilder() {
        return aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace;
    }

    void processNode(const aiNode* node, const aiScene* scene) {
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            processMesh(mesh, scene);
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    void processMesh(const aiMesh* mesh, const aiScene* scene) {
        Mesh mesh_{ _camera };

        std::vector<glm::vec3> positions;
        std::vector<glm::vec4> colors;
        std::vector<glm::vec2> textureCoords;
        std::vector<glm::vec3> normals;
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            glm::vec3 position;
            glm::vec4 color;
            glm::vec2 textureCoord;
            glm::vec3 normal;

            if (mesh->HasPositions()) {
                convertVector(mesh->mVertices[i], position);
                positions.emplace_back(std::move(position));
            }
            if (mesh->HasVertexColors(0)) {
                convertVector(mesh->mColors[0][i], color);
                colors.emplace_back(std::move(color));
            }
            if (mesh->HasTextureCoords(0)) {
                convertVector(mesh->mTextureCoords[0][i], textureCoord);
                textureCoords.emplace_back(std::move(textureCoord));
            }
            if (mesh->HasNormals()) {
                convertVector(mesh->mNormals[i], normal);
                normals.emplace_back(std::move(normal));
            }
            if (mesh->HasBones()) {
            }
            if (mesh->HasFaces()) {
            }
            if (mesh->HasTangentsAndBitangents()) {
            }
            if (mesh->HasTextureCoordsName(0)) {
            }
        }
        mesh_.addPositions(std::move(positions));
        mesh_.addColors(std::move(colors));
        mesh_.addTextureCoords(std::move(textureCoords));
        mesh_.addNormals(std::move(normals));

        std::vector<unsigned int> indices;
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.emplace_back(face.mIndices[j]);
            }
        }
        mesh_.addIndices(std::move(indices));

        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> normalMaps = std::move(loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"));
        std::vector<Texture> heightMaps = std::move(loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height"));
        std::vector<Texture> diffuseMaps = std::move(loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"));
        std::vector<Texture> specularMaps = std::move(loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"));
        mesh_.addTextures(std::move(heightMaps));
        mesh_.addTextures(std::move(normalMaps));
        mesh_.addTextures(std::move(diffuseMaps));
        mesh_.addTextures(std::move(specularMaps));

        mesh_.setShader(_shader);
        mesh_.initMVP();
        mesh_.init();
        _meshes.emplace_back(std::move(mesh_));
    }

    std::vector<Texture> loadMaterialTextures(const aiMaterial* material, const aiTextureType textureType, const std::string& textureTypeName) {
        std::vector<Texture> textures;

        for (unsigned int i = 0; i < material->GetTextureCount(textureType); i++) {
            aiString aiTexturePath;
            material->GetTexture(textureType, i, &aiTexturePath);
            std::string texturePath = _modelDirectory + std::string(aiTexturePath.C_Str());

            Texture texture;
            texture.init(texturePath);
            texture.setTypeName(textureTypeName);
            textures.emplace_back(std::move(texture));
        }

        return textures;
    }

    void static convertVector(const aiVector2D& aiVec, glm::vec2& glmVec) {
        glmVec.x = aiVec.x;
        glmVec.y = aiVec.y;
    }

    void static convertVector(const aiVector3D& aiVec, glm::vec2& glmVec) {
        glmVec.x = aiVec.x;
        glmVec.y = aiVec.y;
    }

    void static convertVector(const aiVector3D& aiVec, glm::vec3& glmVec) {
        glmVec.x = aiVec.x;
        glmVec.y = aiVec.y;
        glmVec.z = aiVec.z;
    }

    void static convertVector(const aiColor4D& aiVec, glm::vec4& glmVec) {
        glmVec.r = aiVec.r;
        glmVec.g = aiVec.g;
        glmVec.b = aiVec.b;
        glmVec.a = aiVec.a;
    }

    std::vector<Mesh> _meshes;
    std::string _modelDirectory;

};
