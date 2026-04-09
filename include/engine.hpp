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
class Engine{
    public:
    enum class EngineMode: uint32_t {
        GAME, 
        EDITOR,
    };
    
    PlatformGLFW plt{};
    AssetManager ast{};
    VulkanStack stk{};
    ShaderCompiler shaderCompiler{};
    ModelStorage modelStorage{};
    FileWatcher fileWatcher{};
    FileReader fileReader{};

    void run();

    void initialize();

    void updateGame(Scene &scn, float aspect,Input::State &state, ModelStorage& storage);
};