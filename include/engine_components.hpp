#pragma once
//#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "ecs_registry.hpp"
#include "input_format.hpp"
#include "memory"
#include "game_memory.hpp"
#include "string"

#include "reflections.hpp"


struct Transform {
    //glm::mat4 model{1.0f};
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
    glm::vec3 scale{ 1.0f };

    glm::mat4 matrix() const {
        glm::mat4 m{ 1.0f };
        m = glm::translate(m, position);
        glm::quat qRotation = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));
        m = m * glm::mat4_cast(qRotation);
        m = glm::scale(m, scale);
        return m;
    }

    REFLECT_3(position, rotation, scale);


};
struct Camera {
    glm::mat4 model{};
    glm::mat4 view{};
    glm::mat4 proj{};
    glm::vec3 eye = glm::vec3(0.0f, 0.0f, 2.0f);
    glm::vec3 dir = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    float pitch{};
    float yaw{ };
};

struct IScript{
    virtual ~IScript() = default;
    virtual void init(ECS::Registry& reg, GameContext& ctx) = 0;
    virtual void update(float aspect, float dt, Input::State& state, ECS::Registry& reg, GameContext& ctx) = 0;
};

struct Script{
    IScript* ptr{};
};

struct Renderable{
    uint32_t id{};
    REFLECT_1(id);
};

struct Parent{
    uint32_t parentID{};
    uint32_t level{};
};

struct Name{
 std::string name{};
};
    
    
