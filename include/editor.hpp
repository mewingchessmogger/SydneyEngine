#pragma once
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h" 
#include "vk_context.hpp"
#include "platform_glfw.hpp"
#include "game_memory.hpp"
#include "ecs_registry.hpp"
#include "engine_components.hpp"
#include <functional>
#include "glm/common.hpp"
#include "vk_mmu.hpp"

class Editor{
    const uint32_t SWAPCHAIN_IMAGE_COUNT = 2;
    
    std::array<vk::ImageView, 2> cachedViews = {};
    std::array<vk::DescriptorSet, 2> cachedDescriptors = {};
    
    public:
    int selectedEntity = -1;
    bool isFirstFrame = true;
    vk::DescriptorPool pool{};    
    vk::DescriptorSet viewportDescSet{};
    ImGuiContext* edtContext{};
    void init(VulkanContext& ctx, PlatformGLFW &plt, vk::Format swapchainFormat);
    void updateEditorInput();
    void evalViewport(vk::Sampler sampler, std::vector<AllocatedImage> &inputTargets);

    void messingAround(vk::CommandBuffer buffer, ECS::Registry &reg, GameContext &ctx, uint32_t currentImgIndex, Camera &edtCam, PlatformGLFW &plt);

    void updateCamera(Camera &cam, PlatformGLFW& plt, float sens);
};