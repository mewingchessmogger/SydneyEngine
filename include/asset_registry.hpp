#pragma once
#include <map>
#include "glm/mat4x4.hpp"
#include "vertex_def.hpp"
#include <string>
#include <variant>
#include "assimp/matrix4x4.h"
#include <array>
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
        std::string name{};
        uint32_t baseIndexLocalIBO{};
        uint32_t indexCount{};
        
        // uint32_t inverseBindMatrixOffset{}; // Where this asset's matrices start in the flat vector
        // uint32_t boneCount{};
    };
    struct AnimData{
        std::string name{};
		uint32_t offsetInLocalBoneBuffer{}; // after how many mat4s does the next animation start?  [0] = 0, [1] = 128 [2] = 2500
        float duration{};
		uint16_t totalFrames{};
        uint16_t boneCount{};

	};
    
    struct SkinnedModel{
        std::string name{};
        std::vector<SkinnedVertex> transientVertices{};
        std::vector<uint32_t> transientIndices{};
        std::vector<SkinnedMeshData> meshes{};
        std::map<std::string, uint32_t> boneNameToIndexMap{};
        glm::mat4 normalizeMat{1.0f};

        aiMatrix4x4 globalInverseTransform{};       
        uint32_t boneCount{};
        std::vector<AnimData> animationsData{};
        std::vector<glm::mat4> transientBones{};
        std::vector<aiMatrix4x4> boneOffsetMats{};
        
// struct Model{
// 	struct AnimData{
// 		uint16_t frameCount{};
// 		uint32_t offsetInLocalBoneBuffer{}; // after how many mat4s does the next animation start?  [0] = 0, [1] = 128 [2] = 2500
// 	};

// 	std::vector<AnimData> animations 
// 	std::vector<glm::mat4> boneMatrices{};
// 	uint32_t boneCount{};

// 	mat4 getMat(uint32_t a, uint32_t keyFrame, uint16_t boneID){
// 		auto& anim = animations[a];
// 		assert(anim.frameCount > keyFrame);
// 		return boneMatrices[anim.offsetInLocalBoneBuffer + keyFrame * boneCount + boneID]; //+ offsetGlobal
// 	}
// }



        uint32_t baseOffsetBytesSkinnedVBO{};
        uint32_t baseOffsetBytesIBO{};
        uint32_t baseOffsetBytesBoneBuffer{};


        int getAnimation(const std::string& animName)  {
            
            for (int a{}; a < animationsData.size(); a++){
                auto& anim = animationsData[a];
                if (animName == anim.name){
                    printf("Found '%s' at index #%d! \n", animName, a);
                    return a;
                }
            }
            // if(animationNameToIndexMap.find(animName) != animationNameToIndexMap.end()){
            //     return animationNameToIndexMap[animName];
            // }
            printf("uh oh %s aint in the map!!\n",animName.c_str());
            assert(0);
            return -1;
        }

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
