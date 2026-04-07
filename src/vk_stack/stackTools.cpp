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
			SET_WIDTH = plt.glwidth;
			SET_HEIGHT = plt.glheight;
            res.rethinkSwapchain(ctx, plt.glwidth, plt.glheight, DESIRED_IMAGES_IN_FLIGHT);
			res.rethinkZBufferImages(ctx, plt.glwidth, plt.glheight,DESIRED_IMAGES_IN_FLIGHT);


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
	
	void VulkanStack::updateUBO(Scene::SceneData& data){
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];
		auto swapchainImage = res.swapchainImages[currentImgIndex];
		std::array< uint32_t,1> dynOffset = {currentFrame * res.strideOfUBO};		
		DynUBO::Base dyn = DynUBO::Base{}
			.setVert(res.vertexBuffer.address)
			.setIndx(res.indexBuffer.address)
			.setView(data.view)
			.setProj(data.proj);

		std::memcpy(static_cast<uint8_t*>(res.uniformBuffer.allocInfo.pMappedData) + dynOffset[0], &dyn, sizeof(dyn));
	}
	
	void VulkanStack::render(Scene& scn){ 
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];
		auto swapchainImage = res.swapchainImages[currentImgIndex];
		std::array< uint32_t,1> dynOffset = {currentFrame * res.strideOfUBO};		
	
		BarrierMasks masks = BarrierMasks{}
			.setSrcStage(vk::PipelineStageFlagBits2::eTopOfPipe)
			.setDstStage(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setSrcAccess(vk::AccessFlagBits2::eNone)
			.setDstAccess(vk::AccessFlagBits2::eColorAttachmentWrite);


		vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor, masks);

		masks.srcStage = vk::PipelineStageFlagBits2::eTopOfPipe;
		masks.dstStage = vk::PipelineStageFlagBits2::eEarlyFragmentTests;
		masks.srcAccess = vk::AccessFlagBits2::eNone;
		masks.dstAccess = vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
		
		vkutils::setPipelineBarrier(cmdBuffer, res.zBufferImages[currentFrame].handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal, vk::ImageAspectFlagBits::eDepth);



		rdr.beginRenderPass(cmdBuffer, swapchainImage.view, swapchainImage.extent2D,res.zBufferImages[currentFrame]);
		
		cmdBuffer.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics, 
			phongPSO.layout, 
			0,                                // firstSet
			res.descriptorSets[static_cast<size_t>(DescriptorSetType::UBO)], 
			dynOffset
		);
		
		for(auto& object : scn.gameObjects){
			PushC::Model pc{};
			pc.setModel(object.model);
			rdr.recordRender(cmdBuffer, res.indexBuffer.address, res.vertexBuffer.address, phongPSO, pc, swapchainImage.extent2D);
		}

		//rdr.recordDragon(cmdBuffer, res.indexBuffer.address, res.vertexBuffer.address, phongPSO, {}, swapchainImage.extent2D);
		
		
		rdr.endRenderPass(cmdBuffer);
		vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR, vk::ImageAspectFlagBits::eColor);
	}



	void VulkanStack::uploadVBOAndIBO(const std::vector<Vertex> &vertices,const std::vector<uint32_t> &indices){
		     
            if (vertices.empty() || indices.empty()) {
				return;
			}
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

	
