#pragma once
#include "vulkan/vulkan.hpp"
#include "vk_barrier.hpp"
#include "buffer_containers.hpp"
#include "vk_mmu.hpp"
class Renderer {
    public:
    void beginRenderPass(vk::CommandBuffer cmdBuffer, vk::ImageView imgView, vk::Extent2D swapchainExtent, AllocatedImage zBufferImage);
    void recordDraw(vk::CommandBuffer cmdBuffer, vk::Pipeline pipeline, vk::Extent2D swapchainExtent, float extentH, float extentW);

    void recordDragon(vk::CommandBuffer cmdBuffer, uint64_t indxAdress, uint64_t vertAdress, PipelineBundle pipeline, PushC::Base pc, vk::Extent2D extent);
    void recordRender(vk::CommandBuffer cmdBuffer, uint64_t indxAdress, uint64_t vertAdress, PipelineBundle pipeline, PushC::Model pc, vk::Extent2D extent);
    void EvalGraph();

    void endRenderPass(vk::CommandBuffer cmdBuffer){
        cmdBuffer.endRendering();}
};