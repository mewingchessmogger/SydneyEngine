#pragma once 
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "engine_api.hpp"


struct RawMemory{
    void* data;
    uint32_t bytes{};

    RawMemory(uint32_t bytes) : bytes(bytes){
        data = std::malloc(bytes);
    }
     ~RawMemory() {
        std::free(data);
     }
};

struct PassedStructuresDLL{
    ECS::Registry* reg;
    EngineAPI* api;
    Input::State* state;
    float* aspect;
    float* dt;
    RawMemory* mem;
};