#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
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