
#include "vk_mmu.hpp"
#include "vk_barrier.hpp"
#include "vertex_def.hpp"
#include "iostream"

void ResManager::createBuffer(BufferType type, unsigned long byteCapacity, AllocatedBuffer &buffer)
{
	vk::BufferCreateInfo BufferInfo{};
	
	BufferInfo
		.setSize(byteCapacity);
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

		case(BufferType::SKINNED_VBO):
			BufferInfo
			.setUsage(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eShaderDeviceAddress)
			.setSharingMode(vk::SharingMode::eExclusive);
			
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE; 						//keep pointer alive bit
			allocInfo.flags = {};			
		break;

		case(BufferType::BONE_BUFFER):
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
		
			BufferInfo.setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress).setSharingMode(vk::SharingMode::eExclusive);
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST; 						//keep pointer alive bit
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
			allocInfo.requiredFlags =VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
			
	}
	auto result = vmaCreateBuffer(allocator, reinterpret_cast<VkBufferCreateInfo*>(&BufferInfo)
		, &allocInfo, reinterpret_cast<VkBuffer*>(&buffer.handle), &buffer.alloc, &buffer.allocInfo);
	
	buffer.capacityBytes = byteCapacity;


	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed creation of staging buffer");
	}


}

uint32_t ResManager::getCurrStride(vk::DeviceSize sizeofBuffer, vk::DeviceSize minSizeUBO){
	std::cout << "min size of UBO: " << minSizeUBO << " bytes\n";
	uint32_t currStride = sizeofBuffer;
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
	return currStride;

}

void ResManager::createDynamicUBO(vk::Device device, uint32_t uboSize, vk::DeviceSize minSizeUBO,uint32_t desiredImagesInFlight, AllocatedBuffer& bufferUBO){

	uint32_t currStride = getCurrStride(uboSize, minSizeUBO);
	auto byteSize = currStride * desiredImagesInFlight;//final size of desired ubo adjusted for limitations of gpu, my gpu has a min 64 byte ubo
	bufferUBO.stride = currStride;//trackkiings stride so could make one big ubo for all objects per frame
	createBuffer(BufferType::UBO_DYN, byteSize, bufferUBO);
	bufferUBO.address = device.getBufferAddress({bufferUBO.handle});
}

void ResManager::initBuffers(vk::Device device, vk::DeviceSize minSizeUBO, uint32_t desiredImagesInFlight){
	createBuffer(BufferType::VBO,VBO_BYTE_CAPACITY,vertexBuffer);
	createBuffer(BufferType::IBO,IBO_BYTE_CAPACITY,indexBuffer);
	createBuffer(BufferType::SKINNED_VBO, SKINNED_VBO_BYTE_CAPACITY, skinnedVertexBuffer);
	createBuffer(BufferType::BONE_BUFFER, BONE_BUFFER_BYTE_CAPACITY, boneBuffer);


    std::cout << "name of vbo, ibo " << vertexBuffer.handle << ", " << indexBuffer.handle << "\n"; 
	vertexBuffer.address = device.getBufferAddress({vertexBuffer.handle});
	indexBuffer.address = device.getBufferAddress({indexBuffer.handle});
	skinnedVertexBuffer.address = device.getBufferAddress({skinnedVertexBuffer.handle});
	boneBuffer.address = device.getBufferAddress({boneBuffer.handle});

	createDynamicUBO(device,sizeof(DynUBO::Base), minSizeUBO, desiredImagesInFlight, uniformBuffer);
	createDynamicUBO(device,sizeof(DynUBO::BoneMat), minSizeUBO, desiredImagesInFlight, dynUBOs.boneMats);
	createDynamicUBO(device,sizeof(DynUBO::CameraData), minSizeUBO, desiredImagesInFlight, dynUBOs.cameraData);
}



//ONLY USE THIS IN VULKANSTACK
template<typename T>
void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<T> &data,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer) //, uint32_t dstOffset
{

		BarrierMasks mask{};

			mask.srcStage = vk::PipelineStageFlagBits2::eTransfer;
			mask.srcAccess = vk::AccessFlagBits2::eTransferWrite;
			mask.dstStage = vk::PipelineStageFlagBits2::eAllGraphics ;
			mask.dstAccess = vk::AccessFlagBits2::eShaderRead;
		
		//createStagingBuffer(byteSize, stagingBuffer);
		vk::DeviceSize bytesT = data.size() * sizeof(T);

		std::memcpy(stagingBuffer.allocInfo.pMappedData, data.data(), bytesT);
		//flush it down the gpu drain so it gets visible for gpu 
		vmaFlushAllocation(allocator, stagingBuffer.alloc, 0, bytesT);

		vk::BufferCopy region{};																//
		region.setSize(bytesT).setDstOffset(dstBuffer.sizeBytes);																//
		//
		cmdBuffer.copyBuffer(stagingBuffer.handle, dstBuffer.handle, region);	//
		vkutils::setPipelineBarrier(cmdBuffer,dstBuffer.handle, bytesT, mask);
		

		dstBuffer.sizeBytes += bytesT;
}
template void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<uint32_t> &data,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer);
template void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<Vertex> &data,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer);
template void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<SkinnedVertex> &data,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer);
template void ResManager::uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, const std::vector<glm::mat4> &data,AllocatedBuffer& stagingBuffer,AllocatedBuffer& dstBuffer);

