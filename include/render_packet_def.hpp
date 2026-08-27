#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>


 

enum class PipelineType{
    PHONG,
    SKIN_PHONG,
    COCONUT, //DO NOT REMOVE!!! very important
};


enum class Mesh : uint32_t{
    STATIC, SKINNED, COLLIDER, WIREFRAME, COCONUT
};

struct Pkt_PC{
    glm::mat4 modelSpace{1.0};
    uint32_t offsetVBO{};// represent either vbo or skinned vbo
    uint32_t offsetBoneBuffer{};
    int pads[2];
    glm::vec4 color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
};


struct RenderPkt{
    Pkt_PC pc{};
    Mesh type{}; 
    uint32_t offsetIBO{};
    uint32_t indexCount{};  
};



