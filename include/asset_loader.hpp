#pragma once
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <map>
#include <cassert>
#include <array>
#include "vertex_def.hpp"
#include "asset_registry.hpp"
#include "glm/vec3.hpp"
class AssetLoader{
    
    AssetRegistry reg{};
    float targetScale = 0.5;

    static constexpr int MAX_BONES_PER_VERTEX = 4;
    
    enum class ModelType{
        FIXED, SKINNED
    };
    
    struct BoneVertexData{
        
        std::array<int,MAX_BONES_PER_VERTEX> bones{};        
        std::array<float,MAX_BONES_PER_VERTEX> weights{};
        
        void addBoneData(uint32_t boneID, float weight){
            for(int i{}; i < MAX_BONES_PER_VERTEX; i++){
                if (weights[i] == 0.0){
                    bones[i] = boneID;
                    weights[i] = weight;
                    //printf("    bone: %d, weight: %f, id: %d, \n", boneID, weight,i);
                    return;
                }
            }
            assert(0);
        }
    };
    
    
    
    Assimp::Importer importer{};
    std::vector<BoneVertexData> vertexToBones{};
    std::vector<int> meshBaseVertex{};
    std::map<std::string, uint32_t> boneNameToIndexMap{};
    uint32_t mdlCounter{};

    public: 
    void parseMeshes(const aiScene *scn, AssetRegistry::StaticModel& mdl);
    void parseMeshes(const aiScene *scn, AssetRegistry::SkinnedModel &mdl);
    void parseMeshBones(int meshIndex, const aiMesh *mesh);
    void parseBone(int meshIndex, const aiBone *bone);
    void parseScene(const aiScene *scn, std::string &path);
    void loadScene(std::string path);
    int getBoneID(const aiBone *bone);
    //void parseMeshes(const aiScene *scn);
    Vertex parseVertex(const aiMesh *mesh, int i);
    SkinnedVertex parseSkinnedVertex(const aiMesh *mesh, int i);
    AssetRegistry &getAssetReg();
    const aiScene*  getScene(std::string path);


};

