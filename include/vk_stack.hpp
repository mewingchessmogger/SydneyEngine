#pragma once 
#include "vk_renderer.hpp"
#include "vk_mmu.hpp"
#include "platform_glfw.hpp"
#include "vk_context.hpp"
#include "pipeline_builder.hpp"
#include "shared_definitions.hpp"
#include "vertex_def.hpp"
#include "scene.hpp"
#include "asset_registry.hpp"
#include "ecs_registry.hpp"
#include "engine_components.hpp"
class VulkanStack{
  
    public:
        
        uint64_t tailVBO{};
        uint64_t tailSkinnedVBO{};
        uint64_t tailIBO{};
        const int MAX_OBJECTS = 2;
        const int DESIRED_IMAGES_IN_FLIGHT = 2;
        const int DESIRED_DEPTH_IMAGES = 4;
        int WIDTH = 1900;
        int HEIGHT = 1080;
        int SET_WIDTH = 1900;
        int SET_HEIGHT = 1080;
        bool bUseValidationLayers = true;
        bool frameBufferResized = false;
        float deltaT{};
        uint32_t currentImgIndex{};
        int currentFrame{};
        VulkanContext ctx;
        Renderer rdr{};
        ResManager res{};
        PipelineBuilder plb{};
        
        struct {
        PipelineBundle test{};
        PipelineBundle phong{};
        PipelineBundle skinnedPhong{};
        PipelineBundle pick{};
    
        } pipelines;

        std::vector<vk::CommandBuffer> cmdBuffers{};
        
        struct VKSBindDescInfo{
			vk::PipelineBindPoint bind{};
			vk::PipelineLayout layout{};
			uint32_t firstSet{};
			std::array<vk::DescriptorSet,3> descSets{};
			uint32_t dynOffset;
		};
        struct RenderCommand{
            PushC::Model pc{};
            PipelineBundle pipeline{};
            vk::Extent2D extent{};
            uint32_t indiceCount{};
            uint32_t totalOffsetIBO{};
        };

        std::vector<RenderCommand> renderQueue{};

        void initInstance(PlatformGLFW& plt);
        void initDevice(PlatformGLFW& plt);
        void createSwapchain();
        void initCommands();
        void initDescriptorStuff();
        void initUpdateDescriptorSets();
        void initSyncs();
        void initBuffers();
        void initSwapchain();
        void initRenderTargetImages();
        void initViewportImages();
        void ImmediateTransitionViewport();
        void initImGuiViewportImages();
        void initDepthImages();
        void initColorPickImage();
        void initTestPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);

        void initPhongPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);
        void initSkinPhongPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);
        // void initPickPipeline(std::vector<uint32_t> &&vertSpv, std::vector<uint32_t> &&fragSpv);

        PipelineBundle createPipeline(const std::vector<uint32_t> &vertSpv, const std::vector<uint32_t> &fragSpv, CreatePipelineInfo p);

        

        

        bool acquireAndValidateImage(PlatformGLFW &plt);
        
        
        

        void startFrame();

        void startEditorToSwapchain();
        void endEditorToSwapchain();
        void blitTargetToViewport();
        void blitTargetToSwapchain();

        void endFrame();

        void earlyEndFrame();

        void updateUBO(glm::mat4& view, glm::mat4& proj);

        void transitionImage(AllocatedImage &img, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, BarrierMasks masks = {});

        void render(std::vector<Scene::RenderPkt> &pkts, AssetRegistry &astReg);

        void render(Scene &scn, AssetRegistry &astReg);



        bool flushUploads(AssetRegistry &astReg);

    private:
    
        vk::ResultValue<uint32_t> acquiredImage();
        void recordSubmit(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore, vk::PipelineStageFlagBits2 waitStageMask, vk::PipelineStageFlagBits2 signalStageMask, vk::Queue graphicsQueue, vk::Fence fence);
        
        
        template <typename T>
        void uploadToBufferT(const std::vector<T> &data, AllocatedBuffer &dstBuffer);
};