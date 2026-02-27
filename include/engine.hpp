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

class Engine{
    public:
    
    PlatformGLFW plt{};
    AssetManager ast{};
    VulkanStack stk{};
    ShaderCompiler shaderCompiler{};
    ModelStorage modelStorage{};
    FileWatcher fileWatcher{"../../../../src/shaders",5};
    FileReader fileReader{};

    //IO + shadercompiler + tracking changed shader files
    //glfw instance input n window handling
    //asset manager
    void run();

};