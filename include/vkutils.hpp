#include "vulkan/vulkan.hpp"
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
namespace vkutils{

void submitHelper(vk::CommandBuffer cmdBuffer, vk::Semaphore waitSemaphore, vk::Semaphore signalSemaphore,
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

 
void transitionImage(vk::Image image, vk::CommandBuffer cmdBuffer, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,BarrierMasks mask = {}, vk::ImageAspectFlagBits aspectImage = vk::ImageAspectFlagBits::eColor){

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
void bufferBarrier(
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

}
