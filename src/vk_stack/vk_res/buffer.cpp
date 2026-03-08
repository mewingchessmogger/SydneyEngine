
#include "vk_mmu.hpp"
#include "vk_barrier.hpp"
#include "vertex_def.hpp"
#include "iostream"

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
		, &allocInfo, reinterpret_cast<VkBuffer*>(&buffer.handle), &buffer.alloc, &buffer.allocInfo);
		
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed creation of staging buffer");
	}


}


void ResManager::initBuffers(vk::Device device, vk::DeviceSize minSizeUBO, uint32_t desiredImagesInFlight){
	createBuffer(BufferType::VBO,VBO_BYTE_SIZE,vertexBuffer);
	createBuffer(BufferType::IBO,IBO_BYTE_SIZE,indexBuffer);
    std::cout << "name of vbo, ibo " << vertexBuffer.handle << ", " << indexBuffer.handle << "\n"; 
	vertexBuffer.address = device.getBufferAddress({vertexBuffer.handle});
	indexBuffer.address = device.getBufferAddress({indexBuffer.handle});


	//////////////////////////////
	vk::DeviceSize uboStructSize = sizeof(DynUBO::Base);                 // 192
	std::cout << "min size of UBO: " << minSizeUBO << " bytes\n";
	size_t currStride = sizeof(DynUBO::Base);
	//check if ubosize is less than min size 
	if (currStride <= minSizeUBO) {
		currStride = minSizeUBO;
	}
	else {
		auto minStride = minSizeUBO;
		auto dummySize = minSizeUBO;
		while (currStride > dummySize) {
			dummySize += minStride;
		}
		currStride = dummySize;
	}
	//iter over num of frames times

	auto byteSize = currStride * desiredImagesInFlight;//final size of desired ubo adjusted for limitations of gpu, my gpu has a min 64 byte ubo
	strideOfUBO = currStride;//trackkiings stride so could make one big ubo for all objects per frame
	createBuffer(BufferType::UBO_DYN, byteSize, uniformBuffer);
	

}

void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<Vertex> &vertices,vk::DeviceSize byteSize,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer)
{

		BarrierMasks mask{};

			mask.srcStage = vk::PipelineStageFlagBits2::eTransfer;
			mask.srcAccess = vk::AccessFlagBits2::eTransferWrite;
			mask.dstStage = vk::PipelineStageFlagBits2::eAllGraphics ;
			mask.dstAccess = vk::AccessFlagBits2::eShaderRead;
		
		//createStagingBuffer(byteSize, stagingBuffer);

		std::memcpy(stagingBuffer.allocInfo.pMappedData, vertices.data(), byteSize);
		//flush it down the gpu drain so it gets visible for gpu 
		vmaFlushAllocation(allocator, stagingBuffer.alloc, 0, byteSize);

		vk::BufferCopy region{};																//
		region.setSize(byteSize);																//
		//
		cmdBuffer.copyBuffer(stagingBuffer.handle, dstBuffer.handle, region);	//
		vkutils::setPipelineBarrier(cmdBuffer,dstBuffer.handle, byteSize, mask);

}

void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<uint32_t> &indices,vk::DeviceSize byteSize,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer)
{

		BarrierMasks mask{};

			mask.srcStage = vk::PipelineStageFlagBits2::eTransfer;
			mask.srcAccess = vk::AccessFlagBits2::eTransferWrite;
			mask.dstStage = vk::PipelineStageFlagBits2::eAllGraphics ;
			mask.dstAccess = vk::AccessFlagBits2::eShaderRead;
		
		//createStagingBuffer(byteSize, stagingBuffer);

		std::memcpy(stagingBuffer.allocInfo.pMappedData, indices.data(), byteSize);
		//flush it down the gpu drain so it gets visible for gpu 
		vmaFlushAllocation(allocator, stagingBuffer.alloc, 0, byteSize);

		vk::BufferCopy region{};																//
		region.setSize(byteSize);																//
		//
		cmdBuffer.copyBuffer(stagingBuffer.handle, dstBuffer.handle, region);	//
		vkutils::setPipelineBarrier(cmdBuffer,dstBuffer.handle,byteSize, mask);

}
 

