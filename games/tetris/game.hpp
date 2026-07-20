#pragma once 
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"

// #include "glm/gtc/quaternion.hpp"
// #include "glm/gtx/quaternion.hpp"
// #include <glm/gtx/projection.hpp>

// #include "game_memory.hpp"
// #include "reflections.hpp"
#include "engine_api.hpp"
#include "camera.hpp"
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 
// #define  GLM_FORCE_RADIANS
// #define  GLM_ENABLE_EXPERIMENTAL
#include "script_component.hpp"


struct Tetris : IScript{
    
    //GameMemory* memory{};
    void init(ECS::Registry& reg, EngineAPI& api) override;
    void update(float aspect, float dt, Input::State &state, ECS::Registry &reg, EngineAPI& api) override;
};
