
#pragma once 
#include <functional>
#include <deque>
#include "vk_del_queue.hpp"
#include <array>
struct AllocatedBuffer {
    vk::Buffer buffer{};
    VmaAllocation alloc{};
    VmaAllocationInfo allocInfo{};
	vk::DeviceAddress address{};
	vk::DeviceSize offsetInGPU{};
};

struct AllocatedImage {
    vk::Image image{};
    vk::ImageView view;
    VmaAllocation alloc{};
    VmaAllocationInfo allocInfo{};
    vk::Format format;
    vk::Extent3D extent;
};
enum class ImgType: uint32_t{
	DEPTH,
	RENDER_TARGET,
	TEXTURE,
	CUBE_DEPTH,
	CUBE_TEXTURE
};
enum class BufferType: uint32_t{
	STAGING,
	VBO,
	IBO,
	SSBO,
	IDBO,
	UBO_DYN,

};
enum class DescriptorSetType: uint32_t{
	SAMPLER,
	IMAGE,
	UBO,
	COUNT
};
enum class SamplerType : uint32_t {
		TEXTURE,
		DEPTH,
		CUBE_TXT,
		CUBE_DEPTH,
		COUNT,
};



constexpr std::string_view to_string(BufferType type) {
    switch (type) {
        case BufferType::STAGING: return "STAGING";
        case BufferType::VBO:     return "VBO";
        case BufferType::IBO:     return "IBO";
        case BufferType::SSBO:    return "SSBO";
        case BufferType::IDBO:    return "IDBO";
        case BufferType::UBO_DYN: return "UBO_DYN";
        default:                  return "UNKNOWN";
    }
}

class ResManager{
    
	public:

    const uint32_t MAX_SAMPLER = 8;
	const uint32_t MAX_DYN_UBO = 1;
	const uint32_t MAX_IMAGES = 500;
	const uint32_t CURR_DESC_SETS = 3;
	const uint64_t VBO_BYTE_SIZE = 1048576ULL * 250ULL;  //250 MBs
	const uint64_t IBO_BYTE_SIZE = 1048576ULL * 100ULL;  //100 MBs
	std::array<vk::DescriptorSetLayout,static_cast<size_t>(DescriptorSetType::COUNT)> descriptorSetLayouts{};
	std::array<vk::DescriptorSet,static_cast<size_t>(DescriptorSetType::COUNT)> descriptorSets{};
	std::array<uint32_t,static_cast<size_t>(DescriptorSetType::COUNT)> descriptorSetVariableDescriptorCount{0u, MAX_IMAGES, 0u };
	std::array<vk::Sampler, static_cast<size_t>(SamplerType::COUNT)> samplers{};

	vk::DescriptorPool descriptorPool{};
	DeletionQueue delQ{};
	
	VmaAllocator allocator{};
	//add shit to vram

	AllocatedBuffer stagingBuffer{};
	AllocatedBuffer vertexBuffer{};
	AllocatedBuffer indexBuffer{};
	AllocatedBuffer GameObjUBO{};
	std::array<AllocatedBuffer, 2> indirectBuffers{};
	std::array<AllocatedBuffer, 2> idboStagingBuffers{};

	std::array<AllocatedBuffer, 2> StorageBuffers{};
	std::array<AllocatedBuffer, 2> ssboStagingBuffers{};

	std::vector<AllocatedImage> renderTargets{};
	std::vector<AllocatedImage> txtImages{};
	std::vector<AllocatedImage> cubeImages{};
	std::vector<AllocatedImage> zBufferImages{};
	std::vector<AllocatedImage> sMapImages{};
	

    void forgeImage(vk::Device device, vk::Format format, uint32_t width, uint32_t height, vk::ImageUsageFlags imageUsageIntent, AllocatedImage& img, vk::ImageAspectFlags aspectMask, std::string_view type, vk::ImageViewType viewtype, uint32_t arrLayers, bool cubeCompatible);
    void requestImage(vk::Device device, ImgType type, AllocatedImage& img, int imgW, int imgH);
    void recordUploadTextureImage(vk::Device device, vk::CommandBuffer cmdBuffer, AllocatedImage &dstImage, void *texPtr, uint32_t texW, uint32_t texH, uint32_t channels = 4);
    


    void createBuffer(BufferType type, unsigned long byteSize, AllocatedBuffer &buffer);

    void initBuffers(vk::Device device);

    void uploadToBuffer(vk::Device device, vk::CommandBuffer cmdBuffer, void *data, vk::DeviceSize byteSize, AllocatedBuffer &stagingBuffer, AllocatedBuffer &dstBuffer);

    void initDescriptorPoolAndSets(vk::Device device, uint32_t maxImageAmount, uint32_t maxSamplers);
	void initAndUpdateSamplers(vk::Device device, float maxAnisotropy);
    
    
};