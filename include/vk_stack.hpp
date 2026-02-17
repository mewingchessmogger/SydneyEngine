#pragma once 
#include "vk_renderer.hpp"
#include "vk_mmu.hpp"
#include "vk_shader.hpp"
#include "platform_glfw.hpp"
#include "vk_context.hpp"





class VulkanStack{
    private:
    

    public:
        struct BarrierMasks {
    vk::PipelineStageFlags2 srcStage{};
    vk::PipelineStageFlags2 dstStage{};
    vk::AccessFlags2 srcAccess{};
    vk::AccessFlags2 dstAccess{};

    bool notFilled() const {
        return srcStage == vk::PipelineStageFlags2{} &&
            dstStage == vk::PipelineStageFlags2{} &&
            srcAccess == vk::AccessFlags2{} &&
            dstAccess == vk::AccessFlags2{};
    }
};
    uint64_t tailVBO{};
    uint64_t tailIBO{};
    const int MAX_OBJECTS = 2;
    const int NUM_OF_IMAGES = 2;
    const int DESIRED_DEPTH_IMAGES = 4;
    int WIDTH = 1400;
    int HEIGHT = 900;
    bool bUseValidationLayers = true;
    bool frameBufferResized = false;
    float deltaT{};
    uint32_t currentImgIndex{};
    int currentFrame{};
    VulkanContext ctx;
   


    VkRenderer Rdr{};
    ResManager mem{};
    VkShaderManager SdrMgr{};

    void setupRdr();
    void setupMemMgr();

    
    void initInstance(PlatformGLFW& plt);
    void initDevice(PlatformGLFW& plt);
    void createSwapchain();
    void initCommands();
    void initDescriptorStuff();
    void initSyncs();
    void initBuffers();
    
    void submitHelper(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::PipelineStageFlagBits2 waitStageMask, vk::PipelineStageFlagBits2 signalStageMask, vk::Queue graphicsQueue, vk::Fence fence);
    void transitionImage(vk::Image image, vk::CommandBuffer cmdBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, BarrierMasks mask, vk::ImageAspectFlagBits aspectImage);
    void bufferBarrier(vk::Buffer buffer, vk::CommandBuffer cmdBuffer, vk::DeviceSize size, BarrierMasks mask);
};