#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
enum class Effect{
    SKIP = 0x0000,
    INCLUDE_IN_SHADOW_MAP = 0X0001,
    POINT_LIGHT = 0X0002,
    DIR_LIGHT = 0X0004,
    WATER = 0X0010,
    FIRE = 0X0020,
    GRASS = 0X0040,
    DRAW = 0X0080,
    
};
class Scene{
public:

    struct GameObject{
        glm::mat4 model{1.0f};
        uint32_t meshID{};
    };

    
    enum class Mesh : uint32_t{
        STATIC, SKINNED,
    };


    struct Packet{
        glm::mat4 mdl{1.0f};
        Mesh type{};
    };



    std::vector<GameObject> gameObjects{};
};