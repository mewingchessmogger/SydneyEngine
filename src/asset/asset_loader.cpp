    #include "asset_loader.hpp"
    #include "glm/gtc/matrix_transform.hpp"
    #include <glm/gtx/string_cast.hpp>
    #include <iostream>
    #include "glm/gtc/type_ptr.hpp"
    #include "glm/matrix.hpp"      
    #include "string_hasher.hpp"
    void AssetLoader::parseMeshes(const aiScene *scn, AssetRegistry::StaticModel& mdl)
    {
        
        int total_vertices{};
        int total_indices{};
        aiVector3D minVertex{1e10f, 1e10f, 1e10f};
        aiVector3D maxVertex{-1e10f, -1e10f, -1e10f};

        assert(!meshBaseVertex.size());
        meshBaseVertex.resize(scn->mNumMeshes);
        
        

        for(int m{}; m < scn->mNumMeshes; m++){
            const aiMesh* mesh = scn->mMeshes[m];
            AssetRegistry::StaticMeshData meshData{};
            
            int num_vertices    = mesh->mNumVertices;
            int num_indices     = mesh->mNumFaces * 3 ;
            meshBaseVertex[m] = total_vertices;
            
            for(int v{}; v < num_vertices; v++){
                mdl.transientVertices.emplace_back(parseVertex(mesh, v));
            }

            for(int f{}; f < mesh->mNumFaces; f++){
                aiFace face = mesh->mFaces[f];
                for(int i{}; i < face.mNumIndices; i++){
                    mdl.transientIndices.emplace_back(face.mIndices[i] + meshBaseVertex[m]);
                }
            }
            
            
            minVertex.x = (minVertex.x > mesh->mAABB.mMin.x) ? mesh->mAABB.mMin.x : minVertex.x;
            minVertex.y = (minVertex.y > mesh->mAABB.mMin.y) ? mesh->mAABB.mMin.y : minVertex.y;
            minVertex.z = (minVertex.z > mesh->mAABB.mMin.z) ? mesh->mAABB.mMin.z : minVertex.z;
            
            maxVertex.x = (maxVertex.x < mesh->mAABB.mMax.x) ? mesh->mAABB.mMax.x : maxVertex.x;
            maxVertex.y = (maxVertex.y < mesh->mAABB.mMax.y) ? mesh->mAABB.mMax.y : maxVertex.y;
            maxVertex.z = (maxVertex.z < mesh->mAABB.mMax.z) ? mesh->mAABB.mMax.z : maxVertex.z;
            


            meshData.name = mesh->mName.C_Str();
            meshData.indexCount = num_indices;
            meshData.baseIndexLocalIBO = total_indices;
            //printf("mesh #%d Name: '%s', vertices: %d, indices: %d, bones: %d \n", i , mesh->mName.C_Str(), num_vertices,num_indices, num_bones);
            total_vertices +=  num_vertices;
            total_indices  +=  num_indices;
            mdl.meshes.push_back(meshData);
        }
        
        
        //float maxDimension = std::max({maxVertex.x - minVertex.x, maxVertex.y - minVertex.y, maxVertex.z - minVertex.z});
        
       // float desiredScalingFactor = targetScale/maxDimension;
        //mdl.normalizeMat = glm::scale(mdl.normalizeMat, glm::vec3(desiredScalingFactor));
        meshBaseVertex.clear();
        meshBaseVertex.shrink_to_fit();
        //printf("\nstatic_model: %s, meshCount: %d, minP:(%f, %f, %f ), maxP:(%f, %f, %f ), scaleFactor: %f . \n\n", mdl.name.c_str(), scn->mNumMeshes, minVertex.x,minVertex.y, minVertex.z, maxVertex.x,maxVertex.y, maxVertex.z, desiredScalingFactor);
    }

    void AssetLoader::parseMeshes(const aiScene *scn, AssetRegistry::SkinnedModel& mdl){
        int total_vertices{};
        int total_indices{};
        int total_bones{};
        aiVector3D minVertex{1e10f, 1e10f, 1e10f};
        aiVector3D maxVertex{-1e10f, -1e10f, -1e10f};

        assert(!meshBaseVertex.size());
        meshBaseVertex.resize(scn->mNumMeshes);
        

        for(int m{}; m < scn->mNumMeshes; m++){
            const aiMesh* mesh = scn->mMeshes[m];
            AssetRegistry::SkinnedMeshData skinnedMeshData{};
            
            int num_vertices    = mesh->mNumVertices;
            int num_bones = mesh->mNumBones;
            int num_indices     = mesh->mNumFaces * 3 ;
            meshBaseVertex[m] = total_vertices;
            
            for(int v{}; v < num_vertices; v++){
                mdl.transientVertices.emplace_back(parseSkinnedVertex(mesh, v));
            }

            for(int f{}; f < mesh->mNumFaces; f++){
                aiFace face = mesh->mFaces[f];
                for(int i{}; i < face.mNumIndices; i++){
                    mdl.transientIndices.emplace_back(face.mIndices[i] + meshBaseVertex[m]);
                }
            }

            
            for(int b{}; b < num_bones; b++){
                aiBone* bone = mesh->mBones[b];
                
                int boneID = getBoneID(bone);
                for(int w{}; w < bone->mNumWeights; w++){
                    aiVertexWeight weight = bone->mWeights[w];
                    mdl.transientVertices[weight.mVertexId + meshBaseVertex[m]].addBone(boneID, weight.mWeight);
                }
            }
            
            minVertex.x = (minVertex.x > mesh->mAABB.mMin.x) ? mesh->mAABB.mMin.x : minVertex.x;
            minVertex.y = (minVertex.y > mesh->mAABB.mMin.y) ? mesh->mAABB.mMin.y : minVertex.y;
            minVertex.z = (minVertex.z > mesh->mAABB.mMin.z) ? mesh->mAABB.mMin.z : minVertex.z;
            
            maxVertex.x = (maxVertex.x < mesh->mAABB.mMax.x) ? mesh->mAABB.mMax.x : maxVertex.x;
            maxVertex.y = (maxVertex.y < mesh->mAABB.mMax.y) ? mesh->mAABB.mMax.y : maxVertex.y;
            maxVertex.z = (maxVertex.z < mesh->mAABB.mMax.z) ? mesh->mAABB.mMax.z : maxVertex.z;
            

            skinnedMeshData.name = mesh->mName.C_Str();
            skinnedMeshData.indexCount = num_indices;
            skinnedMeshData.baseIndexLocalIBO = total_indices;
            
            total_vertices +=  num_vertices; 
            total_indices  +=  num_indices;
            total_bones += num_bones;
            mdl.meshes.push_back(skinnedMeshData);
        }

        //float maxDimension = std::max({maxVertex.x - minVertex.x, maxVertex.y - minVertex.y, maxVertex.z - minVertex.z});

        //float desiredScalingFactor = targetScale/maxDimension;
        //mdl.normalizeMat = glm::scale(mdl.normalizeMat, glm::vec3(desiredScalingFactor));
        meshBaseVertex.clear();
        meshBaseVertex.shrink_to_fit();
        
        mdl.boneOffsetMats = std::move(boneOffsetMatrices);
        mdl.boneNameToIndexMap = std::move(boneNameToIndexMap);
        
        boneOffsetMatrices.clear();
        boneOffsetMatrices.shrink_to_fit();
        boneNameToIndexMap.clear();
        mdl.bounds = {.max = glm::vec3{maxVertex.x, maxVertex.y, maxVertex.z}, .min = glm::vec3{minVertex.x, minVertex.y, minVertex.z}};
        //printf("\nskinned_model: %s, total bones: %d, meshCount: %d, minP:(%f, %f, %f ), maxP:(%f, %f, %f )", mdl.name.c_str(), boneNameToIndexMap.size(), scn->mNumMeshes, minVertex.x,minVertex.y, minVertex.z, maxVertex.x,maxVertex.y, maxVertex.z);

    }

    SkinnedVertex AssetLoader::parseSkinnedVertex(const aiMesh *mesh, int i){
            SkinnedVertex v{};
            v.normal = {1.0,0.0,0.0};
            if(mesh->HasPositions()){
                aiVector3D& pos = mesh->mVertices[i];
                v.pos = {pos.x, pos.y, pos.z};
            }
            
            if(mesh->HasNormals()){
                aiVector3D& normal = mesh->mNormals[i];
                v.normal = {normal.x, normal.y, normal.z};
            }

            if(mesh->HasTextureCoords(0)){
                aiVector3D& uv = mesh->mTextureCoords[0][i];
                v.texCoord = {uv.x, uv.y};
            }
            
            

            return v;           
    }



    int AssetLoader::getBoneID(const aiBone* bone){
        int boneID = 0;
        std::string name{bone->mName.C_Str()};

        if(boneNameToIndexMap.find(name) == boneNameToIndexMap.end()){
            boneID = boneNameToIndexMap.size();
            boneNameToIndexMap[name] = boneID;
            boneOffsetMatrices.push_back(bone->mOffsetMatrix);
        }
        else{
            boneID = boneNameToIndexMap[name];
            
        }
        return boneID;
    }


    void AssetLoader::parseMeshBones(int meshIndex, const aiMesh *mesh){
        for( int i{}; i < mesh->mNumBones; i++){
            parseBone(meshIndex, mesh->mBones[i]);
        }
    }

    void AssetLoader::parseBone(int meshIndex, const aiBone *bone){
        //printf("        Bone #%d '%s' num vertices affected by this bone %d \n", meshIndex, bone->mName.C_Str(), bone->mNumWeights);
        
        int boneID = getBoneID(bone);
        //printf("        bone ID: %d\n", boneID);


        for( int i{}; i < bone->mNumWeights; i++){
            if (i == 0) printf("\n");
            //printf("                %d: vertex id %d weight %.2f\n", i , bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight);
            const aiVertexWeight& vw = bone->mWeights[i];
            uint32_t globalVertexID = meshBaseVertex[meshIndex] + vw.mVertexId;
            //printf("            Vertex id %d", globalVertexID);
            assert(globalVertexID < vertexToBones.size());
            vertexToBones[globalVertexID].addBoneData(boneID, vw.mWeight);
        }
        //printf("\n");
    }


        
    void AssetLoader::parseScene(const  aiScene *scn, std::string& path)
    {
        
        if(!scn->mMeshes[0]->HasBones()){ //its static
            AssetRegistry::StaticModel mdl{};
            mdl.name = path;
            
            parseMeshes(scn, mdl);
            

            if(reg.IntegerToStringStaticModelMap.find(mdlCounter) != reg.IntegerToStringStaticModelMap.end()){
                printf(" THIS SHIT ALREADY IN SKINEND MAP YO MR WHITE!!!\n");
                assert(0);
            }
            //printf("model: %s, id: %d\n", mdl.name.c_str(), mdlCounter);
            
            reg.IntegerToStringStaticModelMap[mdlCounter] = mdl.name;
            reg.StringToIntegerStaticModelMap[mdl.name] = mdlCounter;
            reg.staticModelMap[mdl.name] = std::move(mdl);

            mdlCounter++; 
        }

        else{
            AssetRegistry::SkinnedModel mdl{};//TITANIC AINT STATIC!!!!
            mdl.name = path;
            parseMeshes(scn, mdl);
         
            parseNodes(scn, mdl);

            if(reg.IntegerToStringSkinnedModelMap.find(mdlCounter) != reg.IntegerToStringSkinnedModelMap.end()){
                printf(" THIS SHIT ALREADY IN SKINEND MAP YO MR WHITE!!!\n");
                assert(0);
            }
            printf("model: %s, id: %d\n", mdl.name.c_str(), mdlCounter);
            reg.IntegerToStringSkinnedModelMap[mdlCounter] = mdl.name;
            reg.StringToIntegerSkinnedModelMap[mdl.name] = mdlCounter;
            reg.skinnedModelMap[mdl.name] = std::move(mdl);

            mdlCounter++;
        }       

        importer.FreeScene();

    }
    const aiNodeAnim* AssetLoader::findNodeAnim(const aiAnimation* animation, const std::string nodeName)
        {
            for (uint32_t i = 0; i < animation->mNumChannels; i++)
            {
                const aiNodeAnim* nodeAnim = animation->mChannels[i];
                if (std::string(nodeAnim->mNodeName.data) == nodeName)
                {
                    return nodeAnim;
                }
            }
            return nullptr;
    }


    void AssetLoader::parseNodeHierarchy(float time, int frameTimesBonesPlusAnimOffset ,aiAnimation* pAnimation, const aiNode* pNode, const aiMatrix4x4& parentTransform, AssetRegistry::SkinnedModel& mdl){
        std::string nodeName = std::string(pNode->mName.C_Str());

        aiMatrix4x4 NodeTransformation(pNode->mTransformation);
        const aiNodeAnim* pNodeAnim = nodeCache[pNode];
        if (pNodeAnim)
        {
                // Get interpolated matrices between current and next frame
                aiMatrix4x4 matScale = interpolateScale(time, pNodeAnim);
                aiMatrix4x4 matRotation = interpolateRotation(time, pNodeAnim);
                aiMatrix4x4 matTranslation = interpolateTranslation(time, pNodeAnim);
                NodeTransformation = matTranslation * matRotation * matScale;
        }

        aiMatrix4x4 GlobalTransformation = parentTransform * NodeTransformation;

        if(mdl.boneNameToIndexMap.find(nodeName) != mdl.boneNameToIndexMap.end()){
            int boneID = mdl.boneNameToIndexMap[nodeName];
            aiMatrix4x4 finalMat = mdl.globalInverseTransform * GlobalTransformation* mdl.boneOffsetMats[boneID];
            // aiVector3D scale, pos;
            // aiQuaternion rot;
            // finalMat.Decompose(scale, rot, pos);
            // printf("scale:(%.4f,%.4f,%.4f)\n",scale.x, scale.y, scale.z);    // quite interesting, for both anime girl and my beloved ak47 the scale at finalMat was always 0.01 hmm...
            mdl.transientBones[boneID + frameTimesBonesPlusAnimOffset] = glm::transpose(glm::make_mat4(&finalMat.a1));
        }
        
        for(int i{}; i < pNode->mNumChildren;i++){
            parseNodeHierarchy(time, frameTimesBonesPlusAnimOffset, pAnimation, pNode->mChildren[i], GlobalTransformation, mdl);
        }
        


    }
    //vibe coded, replacing sacha willems string lookup for ptr lookups
    void AssetLoader::buildNodeAnimCache(const aiAnimation* anim, const aiNode* node)
    {
        std::string nodeName(node->mName.C_Str());
        const aiNodeAnim* found = findNodeAnim(anim, nodeName); 
        if (found) nodeCache[node] = found;

        for (uint32_t i = 0; i < node->mNumChildren; i++)
            buildNodeAnimCache(anim, node->mChildren[i]);
    }


    #include <cmath>
    void AssetLoader::parseNodes(const aiScene* scn, AssetRegistry::SkinnedModel& mdl ){
        
        aiMatrix4x4 dummy{};
        // aiMatrix4x4::RotationX(1.57079632679f, dummy);
            
        // glm::mat3 nodeMat = glm::mat3{glm::transpose(glm::make_mat4(&dummy.a1))};
        // mdl.bounds = {.max = nodeMat * mdl.bounds.max, .min = nodeMat * mdl.bounds.min};
        // glm::vec3 max = mdl.bounds.max;
        // glm::vec3 min = mdl.bounds.min;
        //printf("\n model : '%s' minP:(%f, %f, %f ), maxP:(%f, %f, %f )", mdl.name.c_str(), min.x,min.y, min.z, max.x,max.y, max.z);

        for (int i{}; i < scn->mNumAnimations; i++){
            aiAnimation* anim = scn->mAnimations[i];
            
            AssetRegistry::AnimData animData{};
            int boneCount = mdl.boneNameToIndexMap.size();
            mdl.boneCount = boneCount;
            float durationInTicks = (float)anim->mDuration;
            float ticksPerSecond = (anim->mTicksPerSecond != 0) ? anim->mTicksPerSecond : 25.0f;

            animData.name = std::string(anim->mName.C_Str());
            animData.hash = Hasher::stringview(animData.name);
            animData.boneCount = boneCount;
            animData.duration = (float)anim->mDuration / ticksPerSecond; //duration in sec
            
            mdl.globalInverseTransform = scn->mRootNode->mTransformation.Inverse(); 
            
             printf("animation #%d '%-30s', duration (in sec): %f, ticks per second: %f, it has %d channels\n", i, anim->mName.C_Str(), animData.duration, ticksPerSecond, animData.boneCount);
            int estimatedFrames = (animData.duration != 0) ?  60 * animData.duration : 1;

            animData.totalFrames = estimatedFrames;

            animData.offsetInLocalBoneBuffer = mdl.transientBones.size();
            
            mdl.transientBones.resize(mdl.transientBones.size() + estimatedFrames * boneCount);
            
            
           
            
            buildNodeAnimCache(anim,scn->mRootNode);
            aiMatrix4x4 identity{};
            aiMatrix4x4::RotationX(1.57079632679f, identity);
            
            for (uint32_t frame{}; frame < estimatedFrames; frame++){
                float animTime = (durationInTicks > 0.0f) ? (frame / (float)estimatedFrames) * durationInTicks : 0.0f;
                parseNodeHierarchy(animTime,frame * boneCount + animData.offsetInLocalBoneBuffer, anim, scn->mRootNode, identity, mdl);                
            }
            
            nodeCache.clear();
            
            mdl.animationsData.push_back(animData);
            
        }
    }



    const aiScene* AssetLoader::getScene(std::string path)
    {
        const aiScene* scn = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_GenBoundingBoxes);

        return scn;
    }

    void AssetLoader::loadModel(std::string path) {   
        const aiScene* scn = getScene(path);
        parseScene(scn, path);        
    }



    // void AssetLoader::loadScene(const aiScene* scn, std::string path)
    // {
    //     parseScene(scn, path);

    // }

    void AssetLoader::processNodes(aiScene* scn, std::vector<RenderPkt>& packets, RenderPkt pkt)
    {
        aiNode* root = scn->mRootNode;
        
    }


    AssetRegistry& AssetLoader::getAssetReg()
    {
        return reg;
    }


    Vertex AssetLoader::parseVertex(const aiMesh *mesh, int i){
            Vertex v{};
            v.normal = {1.0,0.0,0.0};
            if(mesh->HasPositions()){
                aiVector3D& pos = mesh->mVertices[i];
                v.pos = {pos.x, pos.y, pos.z};
            }
            
            if(mesh->HasNormals()){
                aiVector3D& normal = mesh->mNormals[i];
                v.normal = {normal.x, normal.y, normal.z};
            }

            if(mesh->HasTextureCoords(0)){
                aiVector3D& uv = mesh->mTextureCoords[0][i];
                v.texCoord = {uv.x, uv.y};
            }
            return v;           
    }


