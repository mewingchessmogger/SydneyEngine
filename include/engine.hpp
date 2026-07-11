#pragma once
#include <iostream>
#include "platform_glfw.hpp"
#include "vulkan/vulkan.hpp"
#include "vk_stack.hpp"
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
#include <windows.h>
#include "loader_dll.hpp"
#include "game_memory.hpp"
#include "editor.hpp"

class Engine{

    public:
    enum class EngineMode: uint32_t {
        GAME, 
        EDITOR,
    };

    GameContext ctx{};
    PlatformGLFW plt{};
    VulkanStack stk{};
    ECS::Registry reg{};
    Editor edt{};
    ShaderCompiler shaderCompiler{};
    ModelStorage modelStorage{};
    FileWatcher fileWatcher{};
    FileReader fileReader{};
    LoaderDLL loader{};
    
    void initialize();
    void updateCamera(Camera& cam, EngineMode context, float sens = 0.3);
    void updatePhysics();
    void prepareRenderables(Scene &scn, AssetRegistry &astReg);
    void run();
};