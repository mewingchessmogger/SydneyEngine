#pragma once
//#include <map>
#include "glm/mat4x4.hpp"
#include "vertex_def.hpp"
//#include <string>
//#include <variant>
#include "assimp/matrix4x4.h"
//#include <array>

class AssetRegistry{
    
    public:
    

    struct StaticMeshData{
        glm::mat4 mat{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
    };
    
    struct StaticModel{
        std::string name{};
        std::vector<Vertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<StaticMeshData> meshes{};
        glm::mat4 normalizeMat{1.0f};
        uint32_t baseOffsetBytesVBO{};
        uint32_t baseOffsetBytesIBO{};

        void DestroyTransients(){
            transientVertices.clear();
            transientVertices.shrink_to_fit();
            transientIndices.clear();
            transientIndices.shrink_to_fit();
        }

        void setGlobalOffsets(uint32_t availOffsetGlobalVBO,uint32_t availOffsetGlobalIBO){
            baseOffsetBytesVBO = availOffsetGlobalVBO;
            baseOffsetBytesIBO = availOffsetGlobalIBO;
            
        }
			
    };
    
    struct SkinnedMeshData{
        glm::mat4 mat{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
        
        // uint32_t inverseBindMatrixOffset{}; // Where this asset's matrices start in the flat vector
        // uint32_t boneCount{};
    };
    struct AnimData{
        uint64_t hash{};
		uint32_t offsetInLocalBoneBuffer{}; // after how many mat4s does the next animation start?  [0] = 0, [1] = 128 [2] = 2500
        float duration{};
		uint16_t totalFrames{};
        uint16_t boneCount{};
	};
    
    struct BoneData{
        uint64_t hash{};
        uint32_t boneIndex{};
    };
    struct SkinnedModel{
        std::string name{};
        std::vector<SkinnedVertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<glm::mat4> transientBones{};
        std::vector<SkinnedMeshData> meshes{};
        std::vector<AnimData> animationsData{};
        std::vector<BoneData> boneData{};
        std::vector<std::string> boneNames{};
        glm::mat4 normalizeMat{1.0f};

        uint32_t baseOffsetBytesSkinnedVBO{};
        uint32_t baseOffsetBytesIBO{};
        uint32_t baseOffsetBytesBoneBuffer{};
        uint32_t boneCount{};

        struct AABB{
            glm::vec3 max{1.0};
            glm::vec3 min{1.0};
        } bounds;
        float boundScale = 0.01; // magic number, this is the scale factor of the final mat produced by parsing nodes, check finalMat variablein  parseNodes function in asset_loader.cpp

        std::map<std::string, uint32_t> boneNameToIndexMap{};
        
        aiMatrix4x4 globalInverseTransform{};       
        std::vector<aiMatrix4x4> boneOffsetMats{};
        

        int getAnimation(const uint64_t hash)  {
            
            for (int a{}; a < animationsData.size(); a++){
                auto& anim = animationsData[a];
                
                if (anim.hash == hash){
                    printf("Found '%llu' at index #%d! \n", anim.hash, a);
                    return a;
                }
            }
            printf("uh oh the hash %llu aint corresponding to anything in the map!!\n",hash);
            assert(0);
            return -1;
        }

         uint32_t getBoneIndex(const uint64_t hash)  {
            
            for (int b{}; b < boneData.size(); b++){
                auto& bone = boneData[b];
                
                if (bone.hash == hash){
                    printf("Found '%llu' at index #%d! \n", bone.hash, b);
                    return b;
                }
            }
            printf("uh oh the hash %llu aint corresponding to anything in the map!!\n",hash);
            assert(0);
            return -1;
        }
        // int getBoneIndex(std::string_view str){
        //     uint64_t hash = Hasher::stringview(str);
        //     for(int i {}; i < boneData.size(); i++){
        //         if (boneData[i].hash = hash);
        //             return boneData[i].boneIndex;
        //     }
        // }
        // int getBoneIndex(uint64_t hash){
        //     for(int i {}; i < boneData.size(); i++){
        //         if (boneData[i].hash = hash);
        //             return boneData[i].boneIndex;
        //     }
        // }

        void DestroyTransients(){
            transientVertices.clear();
            transientVertices.shrink_to_fit();
            transientIndices.clear();
            transientIndices.shrink_to_fit();
            transientBones.clear();
            transientBones.shrink_to_fit();
        }

         void setGlobalOffsets(uint32_t availOffsetGlobalSkinnedVBO,uint32_t availOffsetGlobalIBO, uint32_t availOffsetGlobalBonesBuffer){
            baseOffsetBytesSkinnedVBO = availOffsetGlobalSkinnedVBO;
            baseOffsetBytesIBO = availOffsetGlobalIBO;
            baseOffsetBytesBoneBuffer = availOffsetGlobalBonesBuffer;
        }
    };

        std::map<uint32_t, std::string> IntegerToStringStaticModelMap{};
        std::map<uint32_t, std::string> IntegerToStringSkinnedModelMap{};
        std::map<std::string, uint32_t> StringToIntegerStaticModelMap{};
        std::map<std::string, uint32_t> StringToIntegerSkinnedModelMap{};
        
        std::map<std::string, StaticModel> staticModelMap{};
        std::map<std::string, SkinnedModel> skinnedModelMap{};    

    StaticModel& getStaticModelFromID(uint32_t id);
    SkinnedModel& getSkinnedModelFromID(uint32_t id);

    uint32_t getModelID(std::string &path);

    uint32_t getModelID(std::string &&path);

    AssetRegistry::StaticModel &getStaticModelFromString(std::string &&path);

    bool isSkinned(uint32_t id){
        
        if(IntegerToStringStaticModelMap.find(id) != IntegerToStringStaticModelMap.end()){
            return false;
        }
        else if (IntegerToStringSkinnedModelMap.find(id) != IntegerToStringSkinnedModelMap.end()){
            return true;
        }
        assert(0);
        return false;
    }
    
    

};
