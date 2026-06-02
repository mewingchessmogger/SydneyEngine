#pragma once 
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "hash_model.hpp"

#include "fysik_motor.hpp"
#include "engine_components.hpp"

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtx/projection.hpp>

#include "game_mem.hpp"
// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
 
// #define  GLM_FORCE_RADIANS
// #define  GLM_ENABLE_EXPERIMENTAL
struct Tetris : IScript{
    GameMemory* memory{};
    void init(ECS::Registry& reg) override;
    void update(float aspect, float dt, Input::State &state, ECS::Registry &reg) override;
};

// void initGame(ECS::Registry &reg);
// void updateGame(float aspect, float dt, Input::State &state, ModelStorage &storage, ECS::Registry &reg);

