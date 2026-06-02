#pragma once
//#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ecs_registry.hpp"
#include "input_format.hpp"
#include "memory"
struct Transform {
    //glm::mat4 model{1.0f};
    glm::vec3 position{ 0.0f };
    glm::vec3 rotation{ 0.0f, 0.0f, 0.0f };
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
    virtual void init(ECS::Registry& reg) = 0;
    virtual void update(float aspect, float dt, Input::State& state, ECS::Registry& reg) = 0;
};

struct Script{
    std::unique_ptr<IScript> ptr{};
};

struct Renderable{
    uint32_t meshID;
};


    
    
