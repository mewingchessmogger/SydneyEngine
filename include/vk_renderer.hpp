#pragma once
#include "vulkan/vulkan.hpp"
#include "vk_barrier.hpp"
#include "buffer_containers.hpp"
#include "vk_mmu.hpp"
#include "scene.hpp"
class Renderer {
    public:
    void beginRenderPass(vk::CommandBuffer cmdBuffer, vk::ImageView imgView, vk::Extent2D swapchainExtent, AllocatedImage zBufferImage);
    void renderMesh(vk::CommandBuffer cmdBuffer, PipelineBundle pipeline, Scene::Pkt_PC &pc, vk::Extent2D extent, uint32_t indexCount, uint32_t totalOffsetIBO);
    
    void endRenderPass(vk::CommandBuffer cmdBuffer){
        cmdBuffer.endRendering();}
};