#pragma once

#include "vulkan/vulkan.hpp"
#include <vector>
namespace vkb {
    struct Instance;
    struct Device;
}
struct VulkanContext {
    
    vk::Instance instance{};
    vk::DebugUtilsMessengerEXT debugMessenger{};
    vk::SurfaceKHR surface{};
    vk::Device device{};
    vk::PhysicalDevice chosenGPU{};

    struct queueStruct {
        vk::Queue handle{};
        uint32_t famIndex{};
    };
    queueStruct graphicsQueue{};

    vk::SwapchainKHR swapchain{};
    vk::DescriptorPool imguiPool{};
    vk::CommandPool cmdPool{};
    vk::CommandPool immPool{};

    std::vector<vk::CommandBuffer> immBuffers{};

    std::vector<vk::Fence> immFences{};
    std::vector<vk::Fence> fences{};
    std::array<vk::Semaphore, 2> imageReadySemaphores{};
    std::array<vk::Semaphore, 2> renderFinishedSemaphores{};
    vkb::Instance* vkbInstance{};
    vkb::Device* vkbDevice{};

};
//    VkFormat rawSwapchainFormat{};

    // vk::Pipeline phongPipeline{};
    // vk::PipelineLayout phongLayout{};
    // vk::Pipeline skyboxPipeline{};
    // vk::PipelineLayout skyboxLayout{};
    // vk::Pipeline depthSamplingPipeline{};
    // vk::PipelineLayout depthSamplingLayout{};

    // vk::Pipeline pLightPipeline{};
    // vk::PipelineLayout pLightLayout{};
