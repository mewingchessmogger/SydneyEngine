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
    
    public:
    int selectedEntity = -1;
    vk::DescriptorPool pool{};    
    
    ImGuiContext* edtContext{};
    void init(VulkanStack &stk, PlatformGLFW &plt);
    void render(vk::CommandBuffer buffer, ECS::Registry& reg, GameContext& ctx, bool showEditor);
    void buildUI();
};