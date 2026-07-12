#pragma once
#include <iostream>
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
    LoaderDLL loader{};
    AssetLoader ldr{};

    void initialize();
    void updateCamera(Camera& cam, EngineMode context, float sens = 0.3);
    void updatePhysics();
    void readNodeHierarchy(float animationTime, const aiNode *pNode, const aiMatrix4x4 &parentTransform, std::vector<RenderPkt> &packets, RenderPkt templatePkt, AssetRegistry::SkinnedModel &mdl);
    void readNodeHierarchy(float animationTime, aiAnimation *pAnimation, const aiNode *pNode, const aiMatrix4x4 &parentTransform, const aiMatrix4x4 &globalInverseTransform, std::vector<RenderPkt> &packets, RenderPkt templatePkt, AssetRegistry::SkinnedModel &mdl);
    void parseSceneNodes(const aiScene *scn, std::vector<RenderPkt> &packets, RenderPkt templatePkt, AssetRegistry::SkinnedModel &mdl);
    void prepareRenderables(std::vector<RenderPkt> &packets);
    void run();
};