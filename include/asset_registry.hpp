#pragma once
#include <map>
#include "glm/mat4x4.hpp"
#include "vertex_def.hpp"
#include <string>
class AssetRegistry{
    public:
    

    struct StaticMeshData{
        glm::mat4 modelMat{};
        std::string name{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
    };
    
    struct StaticModel{
        std::vector<Vertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<StaticMeshData> meshes{};
        glm::mat4 normalizeMat{1.0f};
        std::string name{};
        uint32_t baseOffsetVBO{};
        uint32_t baseOffsetIBO{};

        void DestroyTransients(){
            transientVertices.clear();
            transientVertices.shrink_to_fit();
            transientIndices.clear();
            transientIndices.shrink_to_fit();
        }

        void setGlobalOffsets(uint32_t availOffsetGlobalVBO,uint32_t availOffsetGlobalIBO){
            baseOffsetVBO = availOffsetGlobalVBO;
            baseOffsetIBO = availOffsetGlobalIBO;
        }
			
    };
    
    struct SkinnedMeshData{
        glm::mat4 mat{};
        std::string name{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};

        // uint32_t inverseBindMatrixOffset{}; // Where this asset's matrices start in the flat vector
        // uint32_t boneCount{};
    };

    struct SkinnedModel{
        std::vector<SkinnedVertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<SkinnedMeshData> meshes{};
        std::vector<glm::mat4> boneMats{};
        glm::mat4 normalizeMat{1.0f};
        std::string name{};

        uint32_t baseOffsetSkinnedVBO{};
        uint32_t baseOffsetIBO{};
        uint32_t baseOffsetBoneBuffer{};

        
        void DestroyTransients(){
            transientVertices.clear();
            transientVertices.shrink_to_fit();
            transientIndices.clear();
            transientIndices.shrink_to_fit();
        }

         void setGlobalOffsets(uint32_t availOffsetGlobalSkinnedVBO,uint32_t availOffsetGlobalIBO){
            baseOffsetSkinnedVBO = availOffsetGlobalSkinnedVBO;
            baseOffsetIBO = availOffsetGlobalIBO;
        }
    };
    
    std::map<uint32_t, std::string> IntegerToStringStaticModelMap{};
    std::map<uint32_t, std::string> IntegerToStringSkinnedModelMap{};
    
    std::map<std::string, StaticModel> staticModelMap{};
    std::map<std::string, SkinnedModel> skinnedModelMap{};    

    StaticModel& getStaticModelFromID(uint32_t id);
    SkinnedModel& getSkinnedModelFromID(uint32_t id);

};