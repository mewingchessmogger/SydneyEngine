#pragma once 
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "engine_api.hpp"

struct PassedStructuresDLL{
    ECS::Registry* reg;
    EngineAPI* api;
    Input::State* state;
    float* aspect;
    float* dt;
};