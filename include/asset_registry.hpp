#pragma once
#include <map>
#include "glm/mat4x4.hpp"
#include "vertex_def.hpp"
#include <string>
class AssetRegistry{
    public:
    

    struct MeshData{
        glm::mat4 modelMat{};
        std::string name{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
    };
    
    struct SkinnedMeshData{
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
        
        uint32_t inverseBindMatrixOffset{}; // Where this asset's matrices start in the flat vector
        uint32_t boneCount{};
    };

    struct StaticModel{
        std::vector<Vertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<MeshData> meshes{};
        
        std::string name{};
        uint32_t baseOffsetGlobalVBO{};
        uint32_t baseOffsetGlobalIBO{};

        void DestroyTransients(){
            transientVertices.clear();
            transientVertices.shrink_to_fit();
            transientIndices.clear();
            transientIndices.shrink_to_fit();
        }
    };
    
    struct SkinnedModel{
        uint32_t baseOffsetSkinnedVBO{};
        uint32_t baseOffsetSkinnedIBO{};

        uint32_t baseOffsetBoneBuffer{};

        std::vector<SkinnedVertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};

        std::vector<SkinnedMeshData> meshes{};
        std::vector<glm::mat4> inverseBindMatrices{};
    };

    std::map<uint32_t, std::string> IntegerToStringStaticModelMap{};
    std::map<std::string, StaticModel> staticModelMap{};
    std::map<std::string, SkinnedModel> skinnedModelMap{};    

    StaticModel& getStaticModelFromID(uint32_t id);
    
};