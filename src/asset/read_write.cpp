#include "asset_loader.hpp"



// ALL YOU GOTTA DO WRITING FILES IS FILE.READ(ADDRES OF THE VARIABLE, VARIABLE SIZE!!) THATS IT

    void AssetLoader::readSkinnedFile(std::string& path, AssetRegistry::SkinnedModel& mdl) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); 
        }

        uint32_t nameLen = 0;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
        mdl.name.resize(nameLen);
        if (nameLen > 0) {
            file.read(&mdl.name[0], nameLen);
        }

        auto readVector = [&file](auto& vec) {
            uint64_t size = 0;
            file.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
            vec.resize(size);
            if (size > 0) {
                file.read(reinterpret_cast<char*>(vec.data()), size * sizeof(vec[0]));
            }
        };

        readVector(mdl.transientVertices);
        readVector(mdl.transientIndices);
        readVector(mdl.transientBones);

        uint64_t meshCount = 0;
        file.read(reinterpret_cast<char*>(&meshCount), sizeof(uint64_t));
        mdl.meshes.resize(meshCount);
        for (auto& mesh : mdl.meshes) {
            file.read(reinterpret_cast<char*>(&mesh.mat), sizeof(glm::mat4));
            file.read(reinterpret_cast<char*>(&mesh.baseIndexLocalIBO), sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&mesh.indexCount), sizeof(uint32_t));
        }

        uint64_t animDataCount = 0;
        file.read(reinterpret_cast<char*>(&animDataCount), sizeof(uint64_t));
        mdl.animationsData.resize(animDataCount);
        for (auto& animData : mdl.animationsData) {
            file.read(reinterpret_cast<char*>(&animData.hash), sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(&animData.offsetInLocalBoneBuffer), sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&animData.duration), sizeof(float));
            file.read(reinterpret_cast<char*>(&animData.totalFrames), sizeof(uint16_t));
            file.read(reinterpret_cast<char*>(&animData.boneCount), sizeof(uint16_t));
        }

        uint64_t boneDataCount = 0;
        file.read(reinterpret_cast<char*>(&boneDataCount), sizeof(uint64_t));
        mdl.boneData.resize(boneDataCount);
        for (auto& data: mdl.boneData){
            file.read(reinterpret_cast<char*>(&data.hash), sizeof(uint64_t));
            file.read(reinterpret_cast<char*>(&data.boneIndex), sizeof(uint32_t));
        }
        
        int64_t boneNameCount = 0;
        file.read(reinterpret_cast<char*>(&boneNameCount), sizeof(uint64_t));
        assert(boneNameCount > 0);
        mdl.boneNames.resize(boneNameCount);
        for (int i {}; i < boneNameCount; i++){
            uint32_t boneNameLength =  0;
            file.read(reinterpret_cast<char*>(&boneNameLength), sizeof(uint32_t));
            if (boneNameLength <= 0){
                printf("this bone aint have a name!!\n");
            }
            mdl.boneNames[i].resize(boneNameLength);
            file.read(mdl.boneNames[i].data(), boneNameLength);
        }
// add bonedata and names


        file.read(reinterpret_cast<char*>(&mdl.normalizeMat), sizeof(glm::mat4));
        file.read(reinterpret_cast<char*>(&mdl.baseOffsetBytesSkinnedVBO), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&mdl.baseOffsetBytesIBO), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&mdl.baseOffsetBytesBoneBuffer), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&mdl.boneCount), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&mdl.bounds.max), sizeof(glm::vec3));
        file.read(reinterpret_cast<char*>(&mdl.bounds.min), sizeof(glm::vec3));
    }
        
    void AssetLoader::writeSkinnedFile(std::string& path, AssetRegistry::SkinnedModel& mdl){
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); // Kunde inte skapa filen (t.ex. saknade mappar)
        }
        uint32_t nameLen =  (mdl.name.size());
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
        if(nameLen > 0)
            file.write(mdl.name.data(), nameLen);
        

        auto writeVector = [&file](const auto& vec){
            uint64_t size = vec.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
            if (size > 0)
                file.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(vec[0]));
        };

        writeVector(mdl.transientVertices);
        writeVector(mdl.transientIndices);
        writeVector(mdl.transientBones);

        uint64_t meshCount = mdl.meshes.size();
        file.write(reinterpret_cast<const char*>(&meshCount), sizeof(uint64_t));
        for (auto& mesh: mdl.meshes){
            file.write(reinterpret_cast<const char*>(&mesh.mat), sizeof(glm::mat4));
            file.write(reinterpret_cast<const char*>(&mesh.baseIndexLocalIBO), sizeof(uint32_t));
            file.write(reinterpret_cast<const char*>(&mesh.indexCount), sizeof(uint32_t));
        }

        uint64_t animDataCount = mdl.animationsData.size();
        file.write(reinterpret_cast<const char*>(&animDataCount), sizeof(uint64_t));
        for (auto& animData: mdl.animationsData){
            file.write(reinterpret_cast<const char*>(&animData.hash), sizeof(uint64_t));
            file.write(reinterpret_cast<const char*>(&animData.offsetInLocalBoneBuffer), sizeof(uint32_t));
            file.write(reinterpret_cast<const char*>(&animData.duration), sizeof(float));
            file.write(reinterpret_cast<const char*>(&animData.totalFrames), sizeof(uint16_t));
            file.write(reinterpret_cast<const char*>(&animData.boneCount), sizeof(uint16_t));
        }

        uint64_t boneDataCount = mdl.boneData.size();
        file.write(reinterpret_cast<const char*>(&boneDataCount), sizeof(uint64_t));
        for (auto& data: mdl.boneData){
            file.write(reinterpret_cast<const char*>(&data.hash), sizeof(uint64_t));
            file.write(reinterpret_cast<const char*>(&data.boneIndex), sizeof(uint32_t));
        }

        int64_t boneNameCount = mdl.boneNames.size();
        file.write(reinterpret_cast<const char*>(&boneNameCount), sizeof(uint64_t));
        for (auto& name: mdl.boneNames){
            
            uint32_t boneNameLength =  name.size();
            file.write(reinterpret_cast<const char*>(&boneNameLength), sizeof(uint32_t));
            if (boneNameLength <= 0){
                printf("this bone aint have a name!!\n");
            }
            file.write(name.data(), boneNameLength);
        }
 
        

        file.write(reinterpret_cast<const char*>(&mdl.normalizeMat), sizeof(glm::mat4));
        file.write(reinterpret_cast<const char*>(&mdl.baseOffsetBytesSkinnedVBO), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&mdl.baseOffsetBytesIBO), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&mdl.baseOffsetBytesBoneBuffer), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&mdl.boneCount), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&mdl.bounds.max), sizeof(glm::vec3));
        file.write(reinterpret_cast<const char*>(&mdl.bounds.min), sizeof(glm::vec3));

    }












    ///////////////////////////////////////////////////////////
    
    void AssetLoader::readStaticFile(std::string& path, AssetRegistry::StaticModel& mdl) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); // Kunde inte öppna filen
        }

        uint32_t nameLen = 0;
        file.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
        mdl.name.resize(nameLen);
        if (nameLen > 0) {
            file.read(&mdl.name[0], nameLen);
        }

        auto readVector = [&file](auto& vec) {
            uint64_t size = 0;
            file.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
            vec.resize(size);
            if (size > 0) {
                file.read(reinterpret_cast<char*>(vec.data()), size * sizeof(vec[0]));
            }
        };

        // 2. Läs vektorer (ordningen måste matcha skrivningen exakt!)
        readVector(mdl.transientVertices);
        readVector(mdl.transientIndices);

        // 3. Läs meshes
        uint64_t meshCount = 0;
        file.read(reinterpret_cast<char*>(&meshCount), sizeof(uint64_t));
        mdl.meshes.resize(meshCount);
        for (auto& mesh : mdl.meshes) {
            file.read(reinterpret_cast<char*>(&mesh.mat), sizeof(glm::mat4));
            file.read(reinterpret_cast<char*>(&mesh.baseIndexLocalIBO), sizeof(uint32_t));
            file.read(reinterpret_cast<char*>(&mesh.indexCount), sizeof(uint32_t));
        }

       

        // 5. Läs övriga fält och AABB
        file.read(reinterpret_cast<char*>(&mdl.normalizeMat), sizeof(glm::mat4));
        file.read(reinterpret_cast<char*>(&mdl.baseOffsetBytesVBO), sizeof(uint32_t));
        file.read(reinterpret_cast<char*>(&mdl.baseOffsetBytesIBO), sizeof(uint32_t));
        // file.read(reinterpret_cast<char*>(&mdl.bounds.max), sizeof(glm::vec3));
        // file.read(reinterpret_cast<char*>(&mdl.bounds.min), sizeof(glm::vec3));
    }
        
    void AssetLoader::writeStaticFile(std::string& path, AssetRegistry::StaticModel& mdl){
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); // Kunde inte skapa filen (t.ex. saknade mappar)
        }
        uint32_t nameLen =  (mdl.name.size());
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
        if(nameLen > 0)
            file.write(mdl.name.data(), nameLen);
        

        auto writeVector = [&file](const auto& vec){
            uint64_t size = vec.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
            if (size > 0)
                file.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(vec[0]));
        };

        writeVector(mdl.transientVertices);
        writeVector(mdl.transientIndices);

        uint64_t meshCount = mdl.meshes.size();
        file.write(reinterpret_cast<const char*>(&meshCount), sizeof(uint64_t));
        for (auto& mesh: mdl.meshes){
            file.write(reinterpret_cast<const char*>(&mesh.mat), sizeof(glm::mat4));
            file.write(reinterpret_cast<const char*>(&mesh.baseIndexLocalIBO), sizeof(uint32_t));
            file.write(reinterpret_cast<const char*>(&mesh.indexCount), sizeof(uint32_t));
        }

        
        file.write(reinterpret_cast<const char*>(&mdl.normalizeMat), sizeof(glm::mat4));
        file.write(reinterpret_cast<const char*>(&mdl.baseOffsetBytesVBO), sizeof(uint32_t));
        file.write(reinterpret_cast<const char*>(&mdl.baseOffsetBytesIBO), sizeof(uint32_t));
        // file.write(reinterpret_cast<const char*>(&mdl.bounds.max), sizeof(glm::vec3));
        // file.write(reinterpret_cast<const char*>(&mdl.bounds.min), sizeof(glm::vec3));

    }
