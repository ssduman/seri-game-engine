#pragma once

#include "Mesh.h"
#include "Entity.h"
#include "Logger.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <vector>

class Model : Entity {
public:
    Model(ICamera* camera) : Entity(camera) {};

    ~Model() override = default;

    void init() override {}

    void update() override {}

    void render() override {}

    void load(const std::string& modelPath) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(modelPath, flagBuilder());
        if (!scene) {
            LOGGER(error, "read model path '" << modelPath << "' failed: " << importer.GetErrorString());
            return;
        }

        LOGGER(info, "read model path '" << modelPath << "' succeeded");
        LOGGER(info, "model mNumMeshes: " << scene->mNumMeshes);
        LOGGER(info, "model mNumMaterials: " << scene->mNumMaterials);
        LOGGER(info, "model mNumAnimations: " << scene->mNumAnimations);
        LOGGER(info, "model mNumTextures: " << scene->mNumTextures);
        LOGGER(info, "model mNumLights: " << scene->mNumLights);
        LOGGER(info, "model mNumCameras: " << scene->mNumCameras);
        LOGGER(info, "model mNumSkeletons: " << scene->mNumSkeletons);
    }

private:
    unsigned int flagBuilder() {
        return aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_JoinIdenticalVertices |
            aiProcess_SortByPType;
    }

    std::vector<Mesh> _meshes;

};
