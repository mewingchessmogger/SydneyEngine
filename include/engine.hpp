#pragma once
#include <iostream>
#include "platform_glfw.hpp"
#include "vulkan/vulkan.hpp"
#include "vk_stack.hpp"
#include "asset_manager.hpp"
#include "hash_model.hpp"
#include <filesystem>
#include <string>
#include "file_watcher.hpp"
#include "file_reader.hpp"
#include "shader_compiler.hpp"
#include "scene.hpp"
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"
class Engine{
    public:
    enum class EngineMode: uint32_t {
        GAME, 
        EDITOR,
    };

    PlatformGLFW plt{};
    AssetManager ast{};
    VulkanStack stk{};
    ECS::Registry reg{};
    ShaderCompiler shaderCompiler{};
    ModelStorage modelStorage{};
    FileWatcher fileWatcher{};
    FileReader fileReader{};
     
    void run();

    void initialize();
    //void updateGame(Scene &scn, float aspect, float dt, Input::State &state, ModelStorage &storage);
    //void updatePhysics(Scene &scn, float dt);
    //void initGame(Scene &scn);
};