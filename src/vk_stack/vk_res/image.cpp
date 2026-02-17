#include <vk_mmu.hpp>
#include "vkutils.hpp"
#include <stb_image.h>
void ResManager::forgeImage(
    vk::Device device,
    vk::Format format,
	uint32_t width, 
	uint32_t height, 
	vk::ImageUsageFlags imageUsageIntent,
	AllocatedImage& img,
	vk::ImageAspectFlags aspectMask,
	std::string_view type, 
	vk::ImageViewType viewtype,
	uint32_t arrLayers, 
	bool cubeCompatible) {

	vk::ImageCreateInfo imageInfo{};

	//vk::Format::eD32Sfloat;
	vk::Extent3D extent{};

	extent.setWidth(width)
		.setHeight(height)
		.setDepth(1);
	uint32_t mipLevels = 1; //+ floor(log2(std::max(ctx->WIDTH, ctx->HEIGHT)));


	imageInfo
		.setImageType(vk::ImageType::e2D)
		.setFormat(format)
		.setExtent(extent)
		.setMipLevels(mipLevels)
		.setArrayLayers(arrLayers)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setTiling(vk::ImageTiling::eOptimal)
		.setUsage(imageUsageIntent)
		.setSharingMode(vk::SharingMode::eExclusive)
		.setInitialLayout(vk::ImageLayout::eUndefined);


	

	VmaAllocationCreateInfo allocImgInfo{};
	allocImgInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	allocImgInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);



	auto result = vmaCreateImage(allocator, imageInfo, &allocImgInfo, reinterpret_cast<VkImage*>(&img.image), &img.alloc, &img.allocInfo);
	
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed allocating type: " + std::string(type) + " images");
	}
	vk::ImageViewCreateInfo info{};
	vk::ImageSubresourceRange subRange{};

	subRange
		.setAspectMask(aspectMask)
		.setBaseMipLevel(0)
		.setLevelCount(1)
		.setBaseArrayLayer(0)
		.setLayerCount(arrLayers);

	info
		.setViewType(viewtype)
		.setImage(img.image)
		.setFormat(format)
		.setSubresourceRange(subRange);

	img.view = device.createImageView(info);


	
}


void ResManager::requestImage(vk::Device device, ImgType type, AllocatedImage& img, int imgW, int imgH) {
    
    vk::Format format;
    vk::ImageUsageFlags usage;
    vk::ImageAspectFlags aspect;
    std::string_view type_name;
    vk::ImageViewType view_type = vk::ImageViewType::e2D;
    uint32_t layers = 1;
    bool cube_compat = false;

    switch (type) {
        case ImgType::DEPTH:
            format      = vk::Format::eD32Sfloat;
            usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
            aspect      = vk::ImageAspectFlagBits::eDepth;
            type_name   = "depth";
            break;

        case ImgType::RENDER_TARGET:
            format      = vk::Format::eR16G16B16A16Sfloat;
            usage       = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
            aspect      = vk::ImageAspectFlagBits::eColor;
            type_name   = "rendertarget";
            break;

        case ImgType::TEXTURE:
            format      = vk::Format::eR8G8B8A8Srgb;
            usage       = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            aspect      = vk::ImageAspectFlagBits::eColor;
            type_name   = "texture";
            break;

        // case ImgType::CUBE_TEXTURE:
        //     format      = vk::Format::eR8G8B8A8Srgb;
        //     usage       = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
        //     aspect      = vk::ImageAspectFlagBits::eColor;
        //     type_name   = "cubemap";
        //     view_type   = vk::ImageViewType::eCube;
        //     layers      = 6;
        //     cube_compat = true;
        //     break;

        default:
            throw std::runtime_error("not valid imgtype type!");
    }

    forgeImage(device, format, imgW, imgH, usage, img, aspect, type_name, view_type, layers, cube_compat);
}

//pngshit
//upload new image  -> process png, create new txt image, fill stagingbuffer with png, fill image with buffer
//update image ->   -> process png, fill stagingbuffer with png, fill image with buffer




void ResManager::recordUploadTextureImage(vk::Device device, vk::CommandBuffer cmdBuffer,AllocatedImage& dstImage, void* texPtr, uint32_t texW,uint32_t texH,uint32_t channels ) {

	//AllocatedBuffer stagingBuffer{};
	uint32_t texSize = channels * texW * texH;
	//createBuffer(BufferType::STAGING, texSize, stagingBuffer); 
	vk::CommandBufferBeginInfo beginInfo{};													//used for copying stage buffer to fast buffer in gpu mem
	beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);						//
	//
	cmdBuffer.begin(beginInfo);														//
	vk::DeviceSize offsetBuffer{};

	memcpy(static_cast<uint8_t*>(stagingBuffer.allocInfo.pMappedData) + offsetBuffer, texPtr, static_cast<size_t>(texSize));

	vmaFlushAllocation(allocator, stagingBuffer.alloc, offsetBuffer, texSize);
	
    


	vk::BufferImageCopy region{};
	//v
	vk::ImageSubresourceLayers source{};
	source.setAspectMask(vk::ImageAspectFlagBits::eColor).setLayerCount(1);

	region.setImageSubresource(source).setImageExtent(vk::Extent3D{texW, texH, 1 }).setBufferOffset(offsetBuffer);												//
	//
	vkutils::transitionImage(dstImage.image, cmdBuffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);

	cmdBuffer.copyBufferToImage(stagingBuffer.buffer, dstImage.image, vk::ImageLayout::eTransferDstOptimal, region);	//
	//

	vkutils::transitionImage(dstImage.image, cmdBuffer, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
							//
	//
	
    //    offset += (level0Size + 15) & ~15;
	//	bufferImageGranularity is the granularity, in bytes, at which buffer or linear image resources, linear or optimal tensor resources, and optimal image
	//	 resources can be bound to adjacent offsets in the same VkDeviceMemory object without aliasing. See Buffer-Image Granularity for more details.
	
	
	// cmdBuffer.end();																	//
	// //
	// vk::SubmitInfo info{};																	//
	// info.setCommandBuffers(cmdBuffer);
	// //vk::Fence uploadFence = ctx->_device.createFence({});
	// ctx->_graphicsQueue.handle.submit(info);// uploadFence
	// ctx->_graphicsQueue.handle.waitIdle();
	// //ctx->_device.waitForFences(uploadFence, VK_TRUE, UINT64_MAX);
	// //ctx->_device.destroyFence(uploadFence);												//waitIdle cheap hack so it doesnt pot collide with buffer in drawFrame()
	// cmdBuffer.reset();
	
	
	//vmaDestroyBuffer(allocator, stagingBuffer.buffer, stagingBuffer.alloc);
}
