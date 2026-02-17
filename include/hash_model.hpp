#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
	struct primitiveData {
        uint32_t vertexOffset{};
        uint32_t indexOffset{};
        uint32_t indexCount{};
        int texIndex = -1;
    };

    struct Mesh {
        std::vector<primitiveData> primitives{};
    };

    struct ModelRecord {
        std::string path;
        std::vector<Mesh> meshes{};
        uint32_t offsetVBO{};
        uint32_t offsetIBO{};
        uint32_t imgBufferOffset; //no shot we get 2 billion images
    };

    
    struct ModelStorage {
        std::unordered_map<std::string, uint32_t> modelHash{};
        std::vector<ModelRecord> models{};
        uint32_t index{};
        
        void storeModel(ModelRecord model) {
            std::cout << model.path << " added..\n";
            models.push_back(model);
            modelHash[model.path] = index;
            index++;

        }
    struct check {
        bool isFound{};
        uint32_t modelID{};
    };

    check getModelID(std::string& modelPath) {
        auto potentialID = modelHash.find(modelPath);
        if (potentialID== modelHash.end()) {
            std::cout << "cant find: " << modelPath <<" hash..\n";
            return { false, 0 };
        }
        return {true, potentialID->second};
    }


    };
