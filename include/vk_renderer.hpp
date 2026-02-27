#pragma once
#include "vulkan/vulkan.hpp"
class Renderer {
    public:
    void beginRenderPass(vk::CommandBuffer cmdBuffer, vk::ImageView imgView, vk::Extent2D swapchainExtent);
    void recordDraw(vk::CommandBuffer cmdBuffer, vk::Pipeline pipeline, vk::Extent2D swapchainExtent, float extentH, float extentW);
};