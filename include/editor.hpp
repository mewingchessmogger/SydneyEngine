#pragma once
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h" 
#include "vk_stack.hpp"
#include "platform_glfw.hpp"
#include "game_memory.hpp"
#include "ecs_registry.hpp"
#include "engine_components.hpp"
#include <functional>
class Editor{
    std::array<vk::ImageView, 2> cachedViews = {};
    std::array<vk::DescriptorSet, 2> cachedDescriptors = {};
    
    public:
    int selectedEntity = -1;
    bool isFirstFrame = true;
    vk::DescriptorPool pool{};    
    vk::DescriptorSet viewportDescSet{};
    ImGuiContext* edtContext{};
    void init(VulkanStack& stk, PlatformGLFW &plt);
    void render(vk::CommandBuffer buffer, ECS::Registry &reg, GameContext &ctx, uint32_t currentFrame);
    void updateEditorInput();
    void evalViewport(vk::Sampler sampler, std::vector<AllocatedImage> &inputTargets);
    
};