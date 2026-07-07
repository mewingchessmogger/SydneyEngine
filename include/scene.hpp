#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>

class Scene{
public:
enum class PipelineType{
    PHONG,
    SKIN_PHONG,
    COCONUT, //DO NOT REMOVE!!! very important
};
    struct GameObject{
        glm::mat4 model{1.0f};
        uint32_t meshID{};
    };

    
    enum class Mesh : uint32_t{
        STATIC, SKINNED, COCONUT
    };

    struct Pkt_PC{
        glm::mat4 modelSpace{1.0};
        uint32_t offsetVBO{};// represent either vbo or skinned vbo
    };
    struct RenderPkt{
        Pkt_PC pc{};
    
        Mesh type{}; 
        uint32_t boneBaseIndex{};
        uint32_t offsetIBO{};
        uint32_t indexCount{};
    };



    std::vector<RenderPkt> packets{};
};