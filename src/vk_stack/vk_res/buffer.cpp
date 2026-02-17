
#include "vk_mmu.hpp"

void ResManager::createBuffer(BufferType type, unsigned long byteSize, AllocatedBuffer &buffer)
{
	vk::BufferCreateInfo BufferInfo{};
	
	BufferInfo
		.setSize(byteSize);
	VmaAllocationCreateInfo allocInfo{};
	
	switch(type){
		case(BufferType::IBO):
			BufferInfo
			.setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress)
			.setSharingMode(vk::SharingMode::eExclusive);

			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; 						//keep pointer alive bit
			allocInfo.flags = {};
			
		break;

		case(BufferType::VBO):
			BufferInfo
			.setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress)
			.setSharingMode(vk::SharingMode::eExclusive);
			
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; 						//keep pointer alive bit
			allocInfo.flags = {};			
		break;

		case(BufferType::SSBO):
			BufferInfo
			.setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress)
			.setSharingMode(vk::SharingMode::eExclusive);
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; 						//keep pointer alive bit
			allocInfo.flags = {};
		break;

		case(BufferType::IDBO):
			BufferInfo.setUsage(vk::BufferUsageFlagBits::eIndirectBuffer | vk::BufferUsageFlagBits::eTransferDst).setSharingMode(vk::SharingMode::eExclusive);
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; 						//keep pointer alive bit
			allocInfo.flags = {};
		break;

		case(BufferType::STAGING):
			BufferInfo.setUsage(vk::BufferUsageFlagBits::eTransferSrc);
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST; 						//keep pointer alive bit
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		break;

		case(BufferType::UBO_DYN):
			BufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer).setSharingMode(vk::SharingMode::eExclusive);
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST; 						//keep pointer alive bit
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocInfo.requiredFlags =VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
			
	}
	auto result = vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&BufferInfo)
		, &allocInfo, reinterpret_cast<VkBuffer*>(&buffer.buffer), &buffer.alloc, &buffer.allocInfo);
		
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed creation of staging buffer");
	}


}


void ResManager::initBuffers(vk::Device device){
	createBuffer(BufferType::VBO,VBO_BYTE_SIZE,vertexBuffer);
	createBuffer(BufferType::IBO,IBO_BYTE_SIZE,indexBuffer);
	
	vertexBuffer.address = device.getBufferAddress({vertexBuffer.buffer});
	indexBuffer.address = device.getBufferAddress({indexBuffer.buffer});
}

void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, void *data,vk::DeviceSize byteSize,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer)
{

		// BarrierMasks mask{};

		// if (type == Buffer::SSBO) {
		// 	mask.srcStage = vk::PipelineStageFlagBits2::eTransfer;
		// 	mask.srcAccess = vk::AccessFlagBits2::eTransferWrite;
		// 	mask.dstStage = vk::PipelineStageFlagBits2::eVertexShader | vk::PipelineStageFlagBits2::eFragmentShader;
		// 	mask.dstAccess = vk::AccessFlagBits2::eShaderRead;
		// }
		// else if (type == Buffer::IDBO) {
		// 	mask.srcStage = vk::PipelineStageFlagBits2::eTransfer;
		// 	mask.srcAccess = vk::AccessFlagBits2::eTransferWrite;
		// 	mask.dstStage = vk::PipelineStageFlagBits2::eDrawIndirect;
		// 	mask.dstAccess = vk::AccessFlagBits2::eIndirectCommandRead;
		// }
		// else {
		// 	throw std::runtime_error("not supported buffer type!!!!!!");
		// }
		
		// //createStagingBuffer(byteSize, stagingBuffer);

		// std::memcpy(stagingBuffer.allocInfo.pMappedData, data, byteSize);
		// //flush it down the gpu drain so it gets visible for gpu 
		// vmaFlushAllocation(_allocator, stagingBuffer.alloc, 0, byteSize);

		// vk::BufferCopy region{};																//
		// region.setSize(byteSize);																//
		// //
		// cmdBuffer.copyBuffer(stagingBuffer.buffer, dstBuffer.buffer, region);	//

		// vkutils::bufferBarrier(dstBuffer.buffer, cmdBuffer,byteSize, mask);

}
 

