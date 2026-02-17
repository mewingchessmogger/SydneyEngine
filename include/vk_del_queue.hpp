
#pragma once
#include <vulkan/vulkan.hpp>
#include "vk_mem_alloc.h"
#include <vector>

struct DeletionQueue {
    // --- VMA Resources (Need Allocator + Handle) ---
    struct VmaBufferBundle { VkBuffer buffer; VmaAllocation alloc; };
    struct VmaImageBundle  { VkImage image;   VmaAllocation alloc; };
    
    std::vector<VmaBufferBundle> vmaBuffers;
    std::vector<VmaImageBundle>  vmaImages;

    // --- Vulkan Resources (Need Device + Handle) ---
    std::vector<vk::ImageView>           imageViews;
    std::vector<vk::Sampler>             samplers;
    std::vector<vk::DescriptorPool>      descPools;
    std::vector<vk::DescriptorSetLayout> descLayouts;
    std::vector<vk::Pipeline>            pipelines;
    std::vector<vk::PipelineLayout>      pipeLayouts;
    std::vector<vk::ShaderModule>        shaders;
    std::vector<vk::Fence>               fences;
    std::vector<vk::Semaphore>           semaphores;
    std::vector<vk::CommandPool>         commandPools;
    std::vector<vk::SwapchainKHR>        swapchains;

    // --- Push Helpers (Overloaded for ease of use) ---
    
    // VMA
    void add(VkBuffer b, VmaAllocation a) { vmaBuffers.push_back({ b, a }); }
    void add(VkImage i, VmaAllocation a)  { vmaImages.push_back({ i, a }); }

    // Standard Handles
    void add(vk::ImageView h)            { imageViews.push_back(h); }
    void add(vk::Sampler h)              { samplers.push_back(h); }
    void add(vk::DescriptorPool h)       { descPools.push_back(h); }
    void add(vk::DescriptorSetLayout h)  { descLayouts.push_back(h); }
    void add(vk::Pipeline h)             { pipelines.push_back(h); }
    void add(vk::PipelineLayout h)       { pipeLayouts.push_back(h); }
    void add(vk::ShaderModule h)         { shaders.push_back(h); }
    void add(vk::Fence h)                { fences.push_back(h); }
    void add(vk::Semaphore h)            { semaphores.push_back(h); }
    void add(vk::CommandPool h)          { commandPools.push_back(h); }
    void add(vk::SwapchainKHR h)         { swapchains.push_back(h); }
    
    // Convenience: Add vector of handles
    template <typename T>
    void add(const std::vector<T>& handles) {
        for (const auto& h : handles) add(h);
    }

    // --- The Executioner ---
    // Takes the device/allocator needed to actually perform the kill.
    void flush(vk::Device device, VmaAllocator allocator) {
        
        device.waitIdle(); // SAFETY FIRST

        // 1. Destroy Pipelines & Shaders (Depend on Layouts)
        for (auto h : pipelines)   device.destroyPipeline(h);
        for (auto h : shaders)     device.destroyShaderModule(h);
        pipelines.clear(); 
        shaders.clear();

        // 2. Destroy Layouts (Depend on Sets)
        for (auto h : pipeLayouts) device.destroyPipelineLayout(h);
        pipeLayouts.clear();

        // 3. Destroy Descriptors (Pools free the Sets automatically)
        for (auto h : descPools)   device.destroyDescriptorPool(h);
        for (auto h : descLayouts) device.destroyDescriptorSetLayout(h);
        descPools.clear(); 
        descLayouts.clear();

        // 4. Destroy Views & Samplers (Depend on Images)
        for (auto h : imageViews)  device.destroyImageView(h);
        for (auto h : samplers)    device.destroySampler(h);
        imageViews.clear(); 
        samplers.clear();

        // 5. Destroy VMA Allocations (The actual memory)
        for (auto& b : vmaBuffers) vmaDestroyBuffer(allocator, b.buffer, b.alloc);
        for (auto& i : vmaImages)  vmaDestroyImage(allocator, i.image, i.alloc);
        vmaBuffers.clear(); 
        vmaImages.clear();

        // 6. Destroy Command Memory
        for (auto h : commandPools) device.destroyCommandPool(h);
        commandPools.clear();

        // 7. Destroy Sync Objects
        for (auto h : fences)      device.destroyFence(h);
        for (auto h : semaphores)  device.destroySemaphore(h);
        fences.clear(); 
        semaphores.clear();

        // 8. Destroy Swapchain (Usually last resource)
        for (auto h : swapchains)  device.destroySwapchainKHR(h);
        swapchains.clear();
    }
};
