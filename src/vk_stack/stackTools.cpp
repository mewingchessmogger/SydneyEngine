#include "vk_stack.hpp"
#include "platform_glfw.hpp"

void VulkanStack::submitHelper(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore,
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


void VulkanStack::transitionImage(vk::Image image, vk::CommandBuffer cmdBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,BarrierMasks mask, vk::ImageAspectFlagBits aspectImage){

	vk::ImageSubresourceRange subRange{};
	subRange.setAspectMask(aspectImage)
		.setLayerCount(vk::RemainingArrayLayers)
		.setBaseMipLevel(0)
		.setLevelCount(vk::RemainingMipLevels)
		.setBaseArrayLayer(0);

	vk::ImageMemoryBarrier2 barrier{};
	barrier
		.setImage(image)
		.setOldLayout(oldLayout)
		.setNewLayout(newLayout)
		.setSubresourceRange(subRange);

	if (mask.notFilled()) {
		barrier.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
			.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setDstAccessMask(vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite);
		
	}
	else {
		barrier
			.setSrcStageMask(mask.srcStage)
			.setSrcAccessMask(mask.srcAccess)
			.setDstStageMask(mask.dstStage)
			.setDstAccessMask(mask.dstAccess);

	}
	
	



	vk::DependencyInfo depInfo{};
	depInfo.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&barrier);

	cmdBuffer.pipelineBarrier2(depInfo);


}
void VulkanStack::bufferBarrier(
	vk::Buffer buffer,
	vk::CommandBuffer cmdBuffer,
	vk::DeviceSize size,
	BarrierMasks mask
) {
	vk::BufferMemoryBarrier2 barrier{};
	barrier.setBuffer(buffer)
		.setOffset(0)
		.setSize(size)
		.setSrcStageMask(mask.srcStage)
		.setDstStageMask(mask.dstStage)
		.setSrcAccessMask(mask.srcAccess)
		.setDstAccessMask(mask.dstAccess);

	vk::DependencyInfo depInfo{};
	depInfo.setBufferMemoryBarrierCount(1)
		.setPBufferMemoryBarriers(&barrier);

	cmdBuffer.pipelineBarrier2(depInfo);
}

	
	vk::ResultValue<uint32_t> VulkanStack::acquiredImage() {
		
		vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];//read below same shtick
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
		//guarantees buffer is done submitting, therefore choose buffer based on fences index"currentFrame"

		auto imgResult = ctx.device.acquireNextImageKHR(ctx.swapchain, 1000000000, imageReadySemaph);
		
		return imgResult;
	}



	bool VulkanStack::acquireAndValidateImage(PlatformGLFW& plt) {
		vk::Fence curFence[] = {ctx.fences[currentFrame] };
		ctx.device.waitForFences(1, curFence, vk::True, 1000000000);


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

	void VulkanStack::concludeFrame() {

		auto imageIndex = currentImgIndex;

		vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];//read below same shtick
		vk::Semaphore renderFinishedSemaph = ctx.renderFinishedSemaphores[imageIndex];
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 

		cmdBuffer.endRendering();

		transitionImage(res.swapchainImages[imageIndex].image, cmdBuffer, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR);
		cmdBuffer.end();

		submitHelper(cmdBuffer, imageReadySemaph, renderFinishedSemaph, vk::PipelineStageFlagBits2::eColorAttachmentOutput
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
		auto image = res.swapchainImages[currentImgIndex];
		BarrierMasks masks;
		masks.srcStage = vk::PipelineStageFlagBits2::eTopOfPipe;
		masks.dstStage = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
		masks.srcAccess = vk::AccessFlagBits2::eNone;
		masks.dstAccess = vk::AccessFlagBits2::eColorAttachmentWrite;
		
		
		transitionImage(image.image, cmdBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, masks);

		rdr.beginRenderPass(cmdBuffer, image.view, image.extent2D);

		rdr.recordDraw(cmdBuffer, testPSO.handle, image.extent2D, image.extent2D.height, image.extent2D.width);

	}