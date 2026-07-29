#pragma once
#include "platform_glfw.hpp"
#include "vulkan/vulkan.hpp"
#include "vk_stack.hpp"
#include <filesystem>
#include <string>
#include "file_watcher.hpp"
#include "file_reader.hpp"
#include "shader_compiler.hpp"
#include "input_format.hpp"
#include "ecs_registry.hpp"
#include "fysik_motor.hpp"
#include "engine_components.hpp"
#include <windows.h>
#include "loader_dll.hpp"
#include "game_memory.hpp"
#include "editor.hpp"
#include "asset_loader.hpp"
#include "render_packet_def.hpp"
#include "engine_api.hpp"
#include "script_component.hpp"
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
    FileWatcher fileWatcher{};
    FileReader fileReader{};
    AssetLoader ldr{};
    EngineAPI api{};
    void initialize();
    void processAPI();

    void propagateNodes();
    void updatePhysics();
   
    void updateAnimations(float dt);
    void prepareRenderables(std::vector<RenderPkt> &packets);
    
    void run();
};