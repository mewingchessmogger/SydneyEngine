#include "vk_stack.hpp"
#include "vertex_def.hpp"
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

	
	bool VulkanStack::flushUploads(AssetRegistry& astReg){

		bool upload = false;
		for(auto& [name, staticModel] : astReg.staticModelMap){
			if(staticModel.transientVertices.empty() || staticModel.transientIndices.empty()){
					continue;
				}
			upload = true;
			printf("[GPU Upload] Static Model: '%s'...\n", staticModel.name.c_str());
				staticModel.setGlobalOffsets(res.vertexBuffer.sizeBytes, res.indexBuffer.sizeBytes);
				
				uploadToBufferT(staticModel.transientVertices, res.vertexBuffer);
				uploadToBufferT(staticModel.transientIndices, res.indexBuffer);
				
				staticModel.DestroyTransients();
		}

		for(auto& [name, skinnedModel] : astReg.skinnedModelMap){
			if(skinnedModel.transientVertices.empty() || skinnedModel.transientIndices.empty()){
				continue;
			}
			upload =true;
			printf("[GPU Upload] Skinned Model: '%s'...\n", skinnedModel.name.c_str());
			skinnedModel.setGlobalOffsets(res.skinnedVertexBuffer.sizeBytes, res.indexBuffer.sizeBytes,res.boneBuffer.sizeBytes);
			
			uploadToBufferT(skinnedModel.transientVertices, res.skinnedVertexBuffer);
			uploadToBufferT(skinnedModel.transientIndices, res.indexBuffer);
			uploadToBufferT(skinnedModel.transientBones, res.boneBuffer);
			skinnedModel.DestroyTransients();

		}
		
		return upload;
	}

bool VulkanStack::acquireAndValidateImage(void (*stallMinimizedWindow)(void* winPtr, int& glwidth, int& glheight, float& aspectRatio), void* winPtr, int& glwidth, int& glheight, bool& frameBufferResized, float& aspectRatio ) {
	vk::Fence curFence[] = {ctx.fences[currentFrame] };
	ctx.device.waitForFences(1, curFence, vk::True, 1000000000);// if gpu is still using cmdbuffer stall cpu so  cpu doesnt write into that cmdbuffer, go when 
		

	//RAAAAAAAAAAAAAAAAAGH I DONT KNOW HOW ELSE TO DELETE ZOMBIE BUFFERS
	static int frame = 0;
	if (!res.zombieBuffers.empty()){
		frame++;
		if(frame == 3){
			for(AllocatedBuffer& i : res.zombieBuffers) { vmaDestroyBuffer(res.allocator, i.handle, i.alloc); }
			res.zombieBuffers.clear();
			frame = 0;
		}
	}

	vk::ResultValue<uint32_t> val = acquiredImage();
	auto imgIndex = val.value;
	auto result = val.result;
	
	
	if(result ==  vk::Result::eErrorOutOfDateKHR || frameBufferResized == true){
		stallMinimizedWindow(winPtr, glwidth, glheight, aspectRatio);
		frameBufferResized = false;
		ctx.device.destroySemaphore(ctx.imageReadySemaphores[currentFrame]);
		ctx.imageReadySemaphores[currentFrame] = ctx.device.createSemaphore(vk::SemaphoreCreateInfo{});

		ctx.device.waitIdle();
		SET_WIDTH = glwidth;
		SET_HEIGHT = glheight;
		res.rethinkSwapchain(ctx, glwidth, glheight, DESIRED_IMAGES_IN_FLIGHT);
		res.rethinkZBufferImages(ctx, glwidth, glheight,DESIRED_IMAGES_IN_FLIGHT);
		//res.rethinkClrPickImage(ctx, glwidth, glheight);
		res.rethinkRenderTargets(ctx, glwidth, glheight,DESIRED_IMAGES_IN_FLIGHT);
		res.rethinkViewportImages(ctx, glwidth, glheight,DESIRED_IMAGES_IN_FLIGHT);
		ImmediateTransitionViewport();
		res.updateViewportDescriptor(ctx);
		return false;
	}
	else if(result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR){
		printf("acquireNextImageKHR failed with: %s\n", vk::to_string(result).c_str());
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

void VulkanStack::startEditorToSwapchain(){
	
	vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 

	auto& swapchainImage = res.swapchainImages[currentImgIndex];
	auto& viewportImage = res.viewportImages[currentImgIndex];
	BarrierMasks masks = BarrierMasks{}
			.setSrcStage(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
			.setSrcAccess(vk::AccessFlagBits2::eColorAttachmentWrite)
			.setDstStage(vk::PipelineStageFlagBits2::eFragmentShader)
			.setDstAccess(vk::AccessFlagBits2::eShaderSampledRead);// these pipeline barriers blew my mind, super cool stuff


	vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,vk::ImageAspectFlagBits::eColor);
	vkutils::setPipelineBarrier(cmdBuffer, viewportImage.handle, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageAspectFlagBits::eColor);
	
	rdr.beginRenderPass(cmdBuffer,swapchainImage.view,swapchainImage.extent2D,{});
}

void VulkanStack::endEditorToSwapchain(){
	vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
	auto& swapchainImage = res.swapchainImages[currentImgIndex];
	auto& viewportImage = res.viewportImages[currentImgIndex];
	

	rdr.endRenderPass(cmdBuffer);

	vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,vk::ImageAspectFlagBits::eColor); 

}


void VulkanStack::blitTargetToViewport(){
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
		auto& viewportImage = res.viewportImages[currentImgIndex];
		auto& renderTarget = res.renderTargetImages[currentImgIndex];



	    vkutils::setPipelineBarrier(cmdBuffer, renderTarget.handle, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor);
		vkutils::setPipelineBarrier(cmdBuffer, viewportImage.handle, vk::ImageLayout::eShaderReadOnlyOptimal, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor);

			// 3. REGIONS: Define how the pixels stretch from the offscreen target to the screen size
		vk::ImageBlit blitRegion{};
		blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcOffsets[1] = vk::Offset3D(renderTarget.extent3D.width, renderTarget.extent3D.height, 1);
		
		blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstOffsets[1] = vk::Offset3D(SET_WIDTH, SET_HEIGHT, 1); // Stretches to current GLFW window size

		// 4. THE ACTUAL BLIT: Scale and copy pixels directly via GPU hardware blocks
		cmdBuffer.blitImage(
			renderTarget.handle, vk::ImageLayout::eTransferSrcOptimal,
			viewportImage.handle, vk::ImageLayout::eTransferDstOptimal,
			1, &blitRegion,
			vk::Filter::eLinear
		);

		
	}

void VulkanStack::blitTargetToSwapchain(){
		vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];//read below same shtick
		vk::Semaphore renderFinishedSemaph = ctx.renderFinishedSemaphores[currentImgIndex];
		vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];//this is indx currentFrame cuz the fence above 
		auto& swapchainImage = res.swapchainImages[currentImgIndex];
		auto& renderTarget = res.renderTargetImages[currentImgIndex];



	    vkutils::setPipelineBarrier(cmdBuffer, renderTarget.handle, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::eTransferSrcOptimal, vk::ImageAspectFlagBits::eColor);
		vkutils::setPipelineBarrier(cmdBuffer, swapchainImage.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, vk::ImageAspectFlagBits::eColor);

			// 3. REGIONS: Define how the pixels stretch from the offscreen target to the screen size
		vk::ImageBlit blitRegion{};
		blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcOffsets[1] = vk::Offset3D(renderTarget.extent3D.width, renderTarget.extent3D.height, 1);
		
		blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstOffsets[1] = vk::Offset3D(SET_WIDTH, SET_HEIGHT, 1); // Stretches to current GLFW window size

		// 4. THE ACTUAL BLIT: Scale and copy pixels directly via GPU hardware blocks
		cmdBuffer.blitImage(
			renderTarget.handle, vk::ImageLayout::eTransferSrcOptimal,
			swapchainImage.handle, vk::ImageLayout::eTransferDstOptimal,
			1, &blitRegion,
			vk::Filter::eLinear
		);

		// 5. BARRIER: Transition the Swapchain Image from Copy Destination to Presentation Mode
		vkutils::setPipelineBarrier(
			cmdBuffer, 
			swapchainImage.handle, 
			vk::ImageLayout::eTransferDstOptimal, 
			vk::ImageLayout::ePresentSrcKHR, 
			vk::ImageAspectFlagBits::eColor
		);

		

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

}void VulkanStack::abortFrame() {
    // 1. Safely close the recording buffer
    cmdBuffers[currentFrame].end();

    // 2. Grab the semaphore that was just signaled by acquireNextImage
    vk::Semaphore imageReadySemaph = ctx.imageReadySemaphores[currentFrame];
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // 3. Submit a dummy block that WAITS on the semaphore so it gets consumed
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBuffers(cmdBuffers[currentFrame])
              .setWaitSemaphores(imageReadySemaph)
              .setWaitDstStageMask(waitStage); // <-- This unsignals the semaphore!

    ctx.graphicsQueue.handle.submit(submitInfo, ctx.fences[currentFrame]);

    // 4. Advance the frame index safely
    currentFrame = (currentFrame + 1) % DESIRED_IMAGES_IN_FLIGHT;
}


void VulkanStack::updateUBO(glm::mat4& view, glm::mat4& proj){

	
	DynUBO::CameraData cam{};
	
	cam.view = view;
	cam.proj = proj;
	uint32_t camOffset = currentFrame * res.dynUBOs.cameraData.stride;
	std::memcpy(static_cast<uint8_t*>(res.dynUBOs.cameraData.allocInfo.pMappedData) + camOffset, &cam, sizeof(cam));

	// uint32_t boneOffset = currentFrame * res.dynUBOs.boneMats.stride;
	// std::memcpy(static_cast<uint8_t*>(res.dynUBOs.boneMats.allocInfo.pMappedData) + boneOffset, bones.data(), sizeof(bones));

	// 

	DynUBO::Base dyn = DynUBO::Base{}
	.setVert(res.vertexBuffer.address)
	.setIndx(res.indexBuffer.address)// dont have to set eacdh frame but whatevs
	.setSkinVert(res.skinnedVertexBuffer.address)
	.setProjAddress(res.dynUBOs.cameraData.address + camOffset)
	.setBoneAddress(res.boneBuffer.address);


	std::array< uint32_t,1> dynOffset = {currentFrame * res.uniformBuffer.stride};		
	std::memcpy(static_cast<uint8_t*>(res.uniformBuffer.allocInfo.pMappedData) + dynOffset[0], &dyn, sizeof(dyn));

	
}

void VulkanStack::transitionImage(AllocatedImage& img, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, BarrierMasks masks){
	vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];
	vkutils::setPipelineBarrier(cmdBuffer, img.handle, oldLayout, newLayout, vk::ImageAspectFlagBits::eColor, masks);
}

void VulkanStack::render(){ 

	
	vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];
	auto& renderTarget = res.renderTargetImages[currentImgIndex];
	std::array< uint32_t,1> dynOffset = {currentFrame * res.uniformBuffer.stride};		
	
	// BarrierMasks masks = BarrierMasks{}
	// 	.setSrcStage(vk::PipelineStageFlagBits2::eTopOfPipe)
	// 	.setDstStage(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
	// 	.setSrcAccess(vk::AccessFlagBits2::eNone)
	// 	.setDstAccess(vk::AccessFlagBits2::eColorAttachmentWrite);// these pipeline barriers blew my mind, super cool stuff


	vkutils::setPipelineBarrier(cmdBuffer, renderTarget.handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor);
	
	// masks = BarrierMasks{}
	// 	.setSrcStage(vk::PipelineStageFlagBits2::eTopOfPipe)
	// 	.setDstStage(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
	// 	.setSrcAccess(vk::AccessFlagBits2::eNone)
	// 	.setDstAccess(vk::AccessFlagBits2::eDepthStencilAttachmentWrite);

	vkutils::setPipelineBarrier(cmdBuffer, res.zBufferImages[currentFrame].handle, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthAttachmentOptimal, vk::ImageAspectFlagBits::eDepth);

	rdr.beginRenderPass(cmdBuffer, renderTarget.view, renderTarget.extent2D,res.zBufferImages[currentFrame]);
	

	// if same pipeline dont bind new, just render otherwise rebinddescirptorsets and pipeline
	vk::Pipeline cachedPipeline{};
	for(auto& pkt: packets){
 		PipelineBundle& pipeline = pipelines.getPipeline(pkt.type);
		
		if (cachedPipeline != pipeline.handle){
			cmdBuffer.bindDescriptorSets(
				vk::PipelineBindPoint::eGraphics, 
				pipeline.layout, 
				0,                                // firstSet
				res.descriptorSets[static_cast<size_t>(DescriptorSetType::UBO)], 
				dynOffset // no descritporsets2 shit cuz u gotta use volk i dont got it working
			);
			cachedPipeline = pipeline.handle;
		}
		
		switch(pkt.type){
			case Mesh::COLLIDER:
				rdr.renderCollider(cmdBuffer, pipeline, pkt.pc, renderTarget.extent2D);
				break;
			
			case Mesh::STATIC: 
				rdr.renderMesh(cmdBuffer, pipeline, pkt.pc,renderTarget.extent2D, pkt.indexCount, pkt.offsetIBO);
				break;
			
			case Mesh::SKINNED: 
 				rdr.renderMesh(cmdBuffer, pipeline, pkt.pc,renderTarget.extent2D, pkt.indexCount, pkt.offsetIBO);
				break;

			default:
				break;

		}

	}

	rdr.endRenderPass(cmdBuffer); 
}



template<typename T>
void VulkanStack::uploadToBufferT(const std::vector<T>& data, AllocatedBuffer& dstBuffer){ //, uint32_t offsetBuffer
	
	AllocatedBuffer stageT{};
	
	res.createBuffer(BufferType::STAGING, data.size() * sizeof(T), stageT);
	res.uploadToBuffer(ctx.device, cmdBuffers[currentFrame], data, stageT, dstBuffer); // function will increase value of  dstBuffer.sizeBytes with data.size() * sizeof(T)
	
	assert(dstBuffer.sizeBytes <= dstBuffer.capacityBytes);// not handling this right now, mr white

	res.zombieBuffers.push_back(stageT);
}

template void VulkanStack::uploadToBufferT(const std::vector<uint32_t>& data, AllocatedBuffer& dstBuffer);
template void VulkanStack::uploadToBufferT(const std::vector<Vertex>& data, AllocatedBuffer& dstBuffer);
template void VulkanStack::uploadToBufferT(const std::vector<SkinnedVertex>& data, AllocatedBuffer& dstBuffer);
template void VulkanStack::uploadToBufferT(const std::vector<glm::mat4>& data, AllocatedBuffer& dstBuffer);