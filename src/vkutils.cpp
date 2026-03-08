#pragma once
#include "vk_barrier.hpp"

namespace vkutils{

void setPipelineBarrier(vk::CommandBuffer cmdBuffer, vk::Image image,  vk::ImageLayout oldLayout, vk::ImageLayout newLayout,vk::ImageAspectFlagBits aspectImage, BarrierMasks mask){

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
		barrier
			.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
			.setDstAccessMask(vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite);

	}
	else {
		barrier
			.setSrcStageMask(mask.srcStage)
			.setDstStageMask(mask.dstStage)
			.setSrcAccessMask(mask.srcAccess)
			.setDstAccessMask(mask.dstAccess);

	}
	vk::DependencyInfo depInfo{};
	
	depInfo.setImageMemoryBarrierCount(1).setPImageMemoryBarriers(&barrier);
	cmdBuffer.pipelineBarrier2(depInfo);
}


void setPipelineBarrier(vk::CommandBuffer cmdBuffer, vk::Buffer buffer,vk::DeviceSize byteSize, BarrierMasks mask) {
	vk::BufferMemoryBarrier2 barrier{};
	barrier.setBuffer(buffer).setSize(byteSize);

	if (mask.notFilled()) {
		barrier
			.setSrcStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setDstStageMask(vk::PipelineStageFlagBits2::eAllCommands)
			.setSrcAccessMask(vk::AccessFlagBits2::eMemoryWrite)
			.setDstAccessMask(vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite);

	}
	else {
		barrier
			.setSrcStageMask(mask.srcStage)
			.setDstStageMask(mask.dstStage)
			.setSrcAccessMask(mask.srcAccess)
			.setDstAccessMask(mask.dstAccess);

	}
	
	vk::DependencyInfo dep{};
	dep.setBufferMemoryBarriers(barrier);
	cmdBuffer.pipelineBarrier2(dep);
}

// void TtGPipelienBarrier(vk::CommandBuffer cmdBuffer,vk::Buffer buffer){
// 	BarrierMasks mask{};
// 	vk::BufferMemoryBarrier2 barrier{};
// 	barrier
// 			.setSrcStageMask(vk::PipelineStageFlagBits2::eTransfer)
// 			.setDstStageMask(vk::PipelineStageFlagBits2::eVertexInput  | vk::PipelineStageFlagBits2::eDrawIndirect)
// 			.setSrcAccessMask(vk::AccessFlagBits2::eTransferWrite)
// 			.setDstAccessMask(vk::AccessFlagBits2::eIndirectCommandRead);
// 	setPipelineBarrier(cmdBuffer, buffer, mask);

// }

};