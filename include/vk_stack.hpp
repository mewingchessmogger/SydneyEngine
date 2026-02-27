#pragma once 
#include "vk_renderer.hpp"
#include "vk_mmu.hpp"
#include "platform_glfw.hpp"
#include "vk_context.hpp"
#include "pipeline_builder.hpp"
#include "shared_definitions.hpp"


class VulkanStack{
  
    public:
        
        uint64_t tailVBO{};
        uint64_t tailIBO{};
        const int MAX_OBJECTS = 2;
        const int DESIRED_IMAGES_IN_FLIGHT = 2;
        const int DESIRED_DEPTH_IMAGES = 4;
        int WIDTH = 1400;
        int HEIGHT = 900;
        bool bUseValidationLayers = true;
        bool frameBufferResized = false;
        float deltaT{};
        uint32_t currentImgIndex{};
        int currentFrame{};
        VulkanContext ctx;
        Renderer rdr{};
        ResManager res{};
        PipelineBuilder plb{};
        PipelineBundle testPSO{};

        std::vector<vk::CommandBuffer> cmdBuffers{};


        void initInstance(PlatformGLFW& plt);
        void initDevice(PlatformGLFW& plt);
        void createSwapchain();
        void initCommands();
        void initDescriptorStuff();
        void initSyncs();
        void initBuffers();
        void initSwapchain();
        void initTestPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);
        
        bool acquireAndValidateImage(PlatformGLFW &plt);
        void render();
        void startFrame();
        void concludeFrame();
    private:
        vk::ResultValue<uint32_t> acquiredImage();
        void submitHelper(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::PipelineStageFlagBits2 waitStageMask, vk::PipelineStageFlagBits2 signalStageMask, vk::Queue graphicsQueue, vk::Fence fence);
        void transitionImage(vk::Image image, vk::CommandBuffer cmdBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, BarrierMasks mask = {}, vk::ImageAspectFlagBits aspectImage = vk::ImageAspectFlagBits::eColor);
        void bufferBarrier(vk::Buffer buffer, vk::CommandBuffer cmdBuffer, vk::DeviceSize size, BarrierMasks mask);
        

};