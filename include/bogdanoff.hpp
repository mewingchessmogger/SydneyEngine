#pragma once 
#include <filesystem>
namespace Bogdanoff{

    void dumpItAgain(std::string& path, const void* memory, uint32_t size){
         std::ofstream file(path, std::ios::binary);

         if(!file.is_open()){
            assert(0);
         }
         file.write(reinterpret_cast<const char*>(memory), size);

    }

    void pumpItAgain(std::string& path, void* memory, uint32_t size){
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); 
        }

         file.read(reinterpret_cast<char*>(memory), size);
    }
};


/*
std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) {
            assert(0); // Kunde inte skapa filen (t.ex. saknade mappar)
        }
        uint32_t nameLen =  (mdl.name.size());
        file.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));


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

*/