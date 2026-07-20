#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

struct Vertex {
    glm::vec3 pos;
    float pad0 = 0.2;
    glm::vec3 normal;
    float pad1 = 1.0;
    glm::vec2 texCoord;
    float pad2;
    float pad3;
    
    bool operator==(const Vertex& other) const {
        return (pos == other.pos) && (normal == other.normal) && (texCoord == other.texCoord);
    }

};

struct SkinnedVertex {
    glm::vec3 pos;
    float pad0 = 0.2;
    glm::vec3 normal;
    float pad1 = 1.0;
    glm::vec2 texCoord;
    float pad2;
    float pad3;
    glm::ivec4 boneIDs{};
    glm::vec4 weights = {0.0f,0.0f,0.0f,0.0f};
    
    void addBone(int boneID, float weight){
        for(int i{}; i < 4; i++){
            if(weights[i] == 0.0f){
                boneIDs[i] = boneID;
                weights[i] = weight;
                return;
            }
        }

        assert(0);
    }
    bool operator==(const SkinnedVertex& other) const {
        return (pos == other.pos) && (normal == other.normal) && (texCoord == other.texCoord);
    }

};