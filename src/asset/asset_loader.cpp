#include "asset_loader.hpp"




const aiScene* AssetLoader::getScene(std::string path)
{
    const aiScene* scn = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices);

    return scn;
}


void AssetLoader::parseMeshes(const aiScene *scn, AssetRegistry::StaticModel& mdl)
{
    printf("Number of meshes: %d \n", scn->mNumMeshes);
    int total_vertices{};
    int total_indices{};
    //int total_bones{};
    assert(!meshBaseVertex.size());
    meshBaseVertex.resize(scn->mNumMeshes);
    
    for(int m{}; m < scn->mNumMeshes; m++){
        const aiMesh* mesh = scn->mMeshes[m];
        AssetRegistry::MeshData meshData{};
        
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
        

        meshData.name = mesh->mName.C_Str();
        meshData.indexCount = num_indices;
        meshData.baseIndexLocalIBO = total_indices;
        //printf("mesh #%d Name: '%s', vertices: %d, indices: %d, bones: %d \n", i , mesh->mName.C_Str(), num_vertices,num_indices, num_bones);
        total_vertices +=  num_vertices;
        total_indices  +=  num_indices;
        mdl.meshes.push_back(meshData);
    }


    
    // for(int i{}; i < scn->mNumMeshes; i++){
    //     const aiMesh* mesh = scn->mMeshes[i];
    //     int num_vertices    = mesh->mNumVertices;
    //     int num_indices     = mesh->mNumFaces *3 ;
    //     int num_bones       = mesh->mNumBones;
    //     meshBaseVertex[i] = total_vertices;
    //     printf("mesh #%d Name: '%s', vertices: %d, indices: %d, bones: %d \n", i , mesh->mName.C_Str(), num_vertices,num_indices, num_bones);
    //     total_vertices +=  num_vertices;
    //     total_indices  +=  num_indices;
    //     total_bones    +=  num_bones;
        
    //     vertexToBones.resize(total_vertices);
        

    //     if(mesh->HasBones()){
    //         parseMeshBones(i, mesh);
    //     }
    // }

    
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

int AssetLoader::getBoneID(const aiBone* bone){
    int boneID = 0;
    std::string name{bone->mName.C_Str()};

    if(boneNameToIndexMap.find(name) == boneNameToIndexMap.end()){
        boneID = boneNameToIndexMap.size();
        boneNameToIndexMap[name] = boneID;
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

AssetRegistry& AssetLoader::getAssetReg()
{
    return reg;
}



void AssetLoader::parseScene(const  aiScene *scn, std::string& path)
{
    AssetRegistry::StaticModel mdl{};

    if(!scn->HasSkeletons()){
        mdl.name = scn->mName.C_Str();
        parseMeshes(scn, mdl);
        printf("MODEL (Scene actually) NAME IS %s !!!\n", mdl.name.c_str());

        if(mdl.name.empty()){
            mdl.name = std::string{"NO NAMER EVERYBODY COME AND SEE! NO NAMER EVERYBODY! #" + modelCounter};
            printf("NO NAMER IN PARSESCENE!!!!\n");
        }

        reg.IntegerToStringStaticModelMap[modelCounter++] = mdl.name;
        reg.staticModelMap[mdl.name] = std::move(mdl);
    }

}

void AssetLoader::loadScene(std::string path)
{
    parseScene(getScene(path), path);

}
