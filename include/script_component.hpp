#pragma once
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "engine_api.hpp"
struct IScript{
    virtual ~IScript() = default;
    virtual void init(ECS::Registry& reg, EngineAPI& api) = 0;
    virtual void update(float aspect, float dt, Input::State& state, ECS::Registry& reg, EngineAPI& api) = 0;
};

struct Script{
    IScript* ptr{};
};
