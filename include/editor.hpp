#pragma once
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h" 
#include "vk_stack.hpp"
#include "platform_glfw.hpp"
#include "game_memory.hpp"
#include "ecs_registry.hpp"
class Editor{
    public:
    
    vk::DescriptorPool pool{};    
 
    
    void init(VulkanStack &stk, PlatformGLFW &plt);
    void render(vk::CommandBuffer buffer, GameContext& ctx);

};