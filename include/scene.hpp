#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
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
    struct MousePos {
        double x{};
        double y{};

    };
    struct Camera {
        MousePos oldPos{};
        MousePos newPos{};
        glm::vec3 eye = glm::vec3(0.0f, 1.0f, 2.0f);
        glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        float pitch{};
        float yaw{ -90.0f };
        glm::mat4 proj{};
        glm::mat4 view{};
    };
    struct Transform {
        glm::vec3 position{ 0.0f };
        glm::vec3 rotation{ 1.0f, 0.0f, 0.0f };
        glm::vec3 scale{ 1.0f };
        
        glm::mat4 matrix() const {
            glm::mat4 m{ 1.0f };
            m = glm::translate(m, position);
            m = glm::rotate(m, rotation.y, { 0,1,0 });
            m = glm::rotate(m, rotation.x, { 1,0,0 });
            m = glm::rotate(m, rotation.z, { 0,0,1 });
            m = glm::scale(m, scale);
            return m;
        }
    };
    
    struct Instance {
        Transform transform{};
        uint32_t modelID{};
        bool visible = true;
    };

    Instance Dragon{};
    Camera camera{};
};