#include "vk_stack.hpp"
#include "platform_glfw.hpp"
#include "vertex_def.hpp"
#include "scene.hpp"
void VulkanStack::recordSubmit(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore,
	vk::PipelineStageFlagBits2 waitStageMask, vk::PipelineStageFlagBits2 signalStageMask,vk::Queue graphicsQueue,vk::Fence fence) {

	vk::SubmitInfo2 subInfo{};
	vk::CommandBufferSubmitInfo cmdInfo{};
	vk::SemaphoreSubmitInfo waitInfo{};
	vk::SemaphoreSubmitInfo signalInfo{};

	cmdInfo.setCommandBuffer(cmdBuffer).setDeviceMask(0);

	waitInfo.setSemaphore(waitSemaphore)
		.setValue(0)
		.setStageMask(waitStageMask)
		.setDeviceIndex(0);
        

	signalInfo.setSemaphore(signalSemaphore)
		.setValue(0)
		.setStageMask(signalStageMask)
		.setDeviceIndex(0);



	subInfo.setWaitSemaphoreInfos(waitInfo)
		.setSignalSemaphoreInfos(signalInfo)
		.setCommandBufferInfos(cmdInfo);

	graphicsQueue.submit2(subInfo, fence);
	}

	
	vk::ResultValue<uint32_t> VulkanStack::acquiredImage() {
		vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];//read below same shtick
	
		auto imgResult = ctx.device.acquireNextImageKHR(ctx.swapchain, 1000000000, imageReadySemaph);// signal semaphore when monitor is done scanning image and its ready to be drawn to 
		
		return imgResult;
	}



	bool VulkanStack::acquireAndValidateImage(PlatformGLFW& plt) {
		vk::Fence curFence[] = {ctx.fences[currentFrame] };
		ctx.device.waitForFences(1, curFence, vk::True, 1000000000);// if gpu is still using cmdbuffer stall cpu so  cpu doesnt write into that cmdbuffer, go when 


		vk::ResultValue<uint32_t> val = acquiredImage();
		auto imgIndex = val.value;
		auto result = val.result;

        if(result ==  vk::Result::eErrorOutOfDateKHR || plt.frameBufferResized == true){
			plt.stallMinimizedWindow();
			plt.frameBufferResized = false;
			ctx.device.destroySemaphore(ctx.imageReadySemaphores[currentFrame]);
			ctx.imageReadySemaphores[currentFrame] = ctx.device.createSemaphore(vk::SemaphoreCreateInfo{});

			ctx.device.waitIdle();
            res.rethinkSwapchain(ctx, plt.width, plt.height, DESIRED_IMAGES_IN_FLIGHT);
			return false;
        }
		else if(result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR){
            throw std::runtime_error("failed to acquire swap chain image!");
        }
		
		currentImgIndex = imgIndex;
		ctx.device.resetFences(curFence);
		return true;
	}



	void VulkanStack::startFrame() {
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
		cmdBuffer.reset();
		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
		cmdBuffer.begin(beginInfo);
	}

	void VulkanStack::endFrame() {

		auto imageIndex = currentImgIndex;

		vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];//read below same shtick
		vk::Semaphore renderFinishedSemaph = ctx.renderFinishedSemaphores[imageIndex];
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
		cmdBuffer.end();

		recordSubmit(cmdBuffer, imageReadySemaph, renderFinishedSemaph, vk::PipelineStageFlagBits2::eColorAttachmentOutput
			, vk::PipelineStageFlagBits2::eAllGraphics, ctx.graphicsQueue.handle, ctx.fences[currentFrame]);
		


		vk::PresentInfoKHR presInfo{};
		presInfo.setWaitSemaphores({ renderFinishedSemaph })
			.setSwapchains(ctx.swapchain)
			.setImageIndices({ imageIndex });
	
	
	
		ctx.graphicsQueue.handle.presentKHR(presInfo);
	
		currentFrame = (currentFrame + 1) % DESIRED_IMAGES_IN_FLIGHT;
	
	}
	
	void VulkanStack::render(){
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];
		auto swapchainImage = res.swapchainImages[currentImgIndex];

		BarrierMasks masks;
		masks.srcStage = vk::PipelineStageFlagBits2::eTopOfPipe;
		masks.dstStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
		masks.srcAccess = vk::AccessFlagBits2::eNone;
		masks.dstAccess = vk::AccessFlagBits2::eColorAttachmentWrite;
		
		vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor, masks);
		rdr.beginRenderPass(cmdBuffer, swapchainImage.view, swapchainImage.extent2D);
		//rdr.recordDraw(cmdBuffer, testPSO.handle, swapchainImage.extent2D, swapchainImage.extent2D.height, swapchainImage.extent2D.width);
		
		
		vk::BindDescriptorSetsInfo info {};
		std::array< uint32_t,1> dynOffset = {currentFrame * res.strideOfUBO};
		info
		.setDynamicOffsets(dynOffset)
		.setFirstSet(0)
		.setDescriptorSets(res.descriptorSets[2])
		.setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
		.setLayout(phongPSO.layout);
	
	// size_t offsetUBO = (ctx->currentFrame * ResMgr->strideUBO);

	// std::memcpy(base + offsetUBO, &ResMgr->dataUBO, sizeof(ResMgr->dataUBO));
		Scene scn{};
		glm::vec3 eye    = glm::vec3(0.5f, 0.0f, 1.0f);
		glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 up     = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::mat4 view   = glm::lookAt(eye, target, up);
		view = glm::lookAt(eye, target, scn.camera.up);
		float aspect = (float)swapchainImage.extent2D.width / (float)swapchainImage.extent2D.height;
		glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

		static float timer = 0.0f;
		timer += 0.01f; 

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model,glm::vec3(2.0));
		model = glm::rotate(model, timer, glm::vec3(0.0f, 1.0f, 0.0f)); // Y-axis
		model = glm::rotate(model, timer * 0.5f, glm::vec3(0.0f, 0.0f, 1.0f)); // Z-axis
		
		DynUBO::Base base{};
		base.model = model;
		base.view = view ;
		base.proj = proj;
		base.indxAdress = res.indexBuffer.address;
		base.vertAdress = res.vertexBuffer.address;


		std::memcpy(static_cast<uint8_t*>(res.uniformBuffer.allocInfo.pMappedData) + dynOffset[0], &base, sizeof(base));
		//cmdBuffer.bindDescriptorSets2KHR(info);
		cmdBuffer.bindDescriptorSets(
    vk::PipelineBindPoint::eGraphics, 
    phongPSO.layout, 
    0,                                // firstSet
    res.descriptorSets[static_cast<size_t>(DescriptorSetType::UBO)], 
    dynOffset
);
		
		rdr.recordDragon(cmdBuffer, res.indexBuffer.address, res.vertexBuffer.address, phongPSO, {}, swapchainImage.extent2D);
		rdr.endRenderPass(cmdBuffer);
		vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);
	}



	void VulkanStack::uploadVBOAndIBO(const std::vector<Vertex> &vertices,const std::vector<uint32_t> &indices){
		     
            
            AllocatedBuffer stageVertex{};
            AllocatedBuffer stageIndex{};
			vk::DeviceSize bytesVertex = vertices.size() * sizeof(Vertex);
			vk::DeviceSize bytesIndex =  indices.size() * sizeof(uint32_t);

            res.createBuffer(BufferType::STAGING, bytesIndex, stageIndex);
            res.createBuffer(BufferType::STAGING, bytesVertex, stageVertex);
            res.uploadToBuffer(ctx.device, cmdBuffers[currentFrame], vertices,bytesVertex, stageVertex, res.vertexBuffer);
            res.uploadToBuffer(ctx.device, cmdBuffers[currentFrame], indices, bytesIndex, stageIndex, res.indexBuffer);
            //need to deestroy later staging 
	}

	
