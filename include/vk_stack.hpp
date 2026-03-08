#pragma once 
#include "vk_renderer.hpp"
#include "vk_mmu.hpp"
#include "platform_glfw.hpp"
#include "vk_context.hpp"
#include "pipeline_builder.hpp"
#include "shared_definitions.hpp"
#include "vertex_def.hpp"

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
        PipelineBundle phongPSO{};

        std::vector<vk::CommandBuffer> cmdBuffers{};


        void initInstance(PlatformGLFW& plt);
        void initDevice(PlatformGLFW& plt);
        void createSwapchain();
        void initCommands();
        void initDescriptorStuff();
        void initUpdateDescriptorSets();
        void initSyncs();
        void initBuffers();
        void initSwapchain();
        void initTestPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);

        void initPhongPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);

        

        PipelineBundle createPipeline(const std::vector<uint32_t> &vertSpv, const std::vector<uint32_t> &fragSpv, CreatePipelineInfo p);

        

        

        bool acquireAndValidateImage(PlatformGLFW &plt);
        
        
        void render();
        void uploadVBOAndIBO(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
        

        void uploadDataToBuffer();
        void startFrame();
        void endFrame();
        

    private:
        vk::ResultValue<uint32_t> acquiredImage();
        void recordSubmit(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::PipelineStageFlagBits2 waitStageMask, vk::PipelineStageFlagBits2 signalStageMask, vk::Queue graphicsQueue, vk::Fence fence);
         

};