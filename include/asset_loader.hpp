#pragma once
#include "asset_loader_interface.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <map>
#include <cassert>
#include <array>
#include "vertex_def.hpp"
#include "asset_registry.hpp"
#include "glm/vec3.hpp"
#include "render_packet_def.hpp"
class AssetLoader : public IAssetLoader{
    public:
    ~AssetLoader() override = default; // Added 'override' here
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
    std::vector<aiMatrix4x4> boneOffsetMatrices{};
    std::unordered_map<const aiNode*, const aiNodeAnim*> nodeCache{};
    uint32_t mdlCounter{};
    
    public: 
    std::vector<const aiScene*> scenes{};
    void parseMeshes(const aiScene *scn, AssetRegistry::StaticModel& mdl);
    void parseMeshes(const aiScene *scn, AssetRegistry::SkinnedModel &mdl);
    void parseMeshBones(int meshIndex, const aiMesh *mesh);
    void parseBone(int meshIndex, const aiBone *bone);
    //void parseSkinned(const aiScene *scn, AssetRegistry::SkinnedModel &mdl);
    void parseScene(const aiScene *scn, std::string &path);
    const aiNodeAnim *findNodeAnim(const aiAnimation *animation, const std::string nodeName);
    void parseNodeHierarchy(float time, int frameTimesBonesPlusAnimOffset, aiAnimation *pAnimation, const aiNode *pNode, const aiMatrix4x4 &parentTransform, AssetRegistry::SkinnedModel &mdl);
    void buildNodeAnimCache(const aiAnimation *anim, const aiNode *node);
    void parseNodes(const aiScene *scn, AssetRegistry::SkinnedModel &mdl);
    
    // void loadModel(const aiScene *scn, std::string path);
    void processNodes(aiScene *scn, std::vector<RenderPkt> &packets, RenderPkt pkt);
    
    int getBoneID(const aiBone *bone);
    //void parseMeshes(const aiScene *scn);
    Vertex parseVertex(const aiMesh *mesh, int i);
    void loadModel(std::string filename, bool tryCaching) override;
    void readSkinnedFile(std::string &path, AssetRegistry::SkinnedModel &mdl);
    void writeSkinnedFile(std::string &path, AssetRegistry::SkinnedModel &mdl);
    void readStaticFile(std::string &path, AssetRegistry::StaticModel &mdl);

    void writeStaticFile(std::string &path, AssetRegistry::StaticModel &mdl);

    SkinnedVertex parseSkinnedVertex(const aiMesh *mesh, int i);
    AssetRegistry &getAssetReg();
    const aiScene*  getScene(std::string path);

    aiMatrix4x4 interpolateTranslation(float time, const aiNodeAnim *pNodeAnim);

    aiMatrix4x4 interpolateRotation(float time, const aiNodeAnim *pNodeAnim);

    aiMatrix4x4 interpolateScale(float time, const aiNodeAnim *pNodeAnim);
};
