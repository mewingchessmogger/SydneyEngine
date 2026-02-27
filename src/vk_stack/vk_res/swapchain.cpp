
	#include "vk_mmu.hpp"
	#include "VkBootstrap.h"
	#include <iostream>
	#include "vk_context.hpp"
	void ResManager::createSwapchain(VulkanContext &ctx, uint32_t width, uint32_t height, uint32_t imagesInFlight) {
		vkb::SwapchainBuilder swapchainBuilder(ctx.chosenGPU, ctx.device, ctx.surface);
		vk::SurfaceFormatKHR formatInfo{};
		formatInfo.format = vk::Format::eB8G8R8A8Srgb;
		formatInfo.colorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

		vkb::Swapchain vkbSwapchain = swapchainBuilder.set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			.set_desired_format(formatInfo)
			.set_desired_extent(width, height)
			.set_required_min_image_count(imagesInFlight)
			.set_desired_min_image_count(imagesInFlight)
			.add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			.build()
			.value();

		ctx.swapchain = vk::SwapchainKHR{ vkbSwapchain.swapchain };
		auto vkbImages = vkbSwapchain.get_images().value();
		auto vkbImageViews = vkbSwapchain.get_image_views().value();

		assert (swapchainImages.size() == 0);
		for (int i{}; i < imagesInFlight; i++) {
			AllocatedImage img{};
			img.image = vkbImages[i];
			img.view = vkbImageViews[i];
			img.format = vk::Format::eB8G8R8A8Srgb;
			img.extent2D = vk::Extent2D{ vkbSwapchain.extent };
			swapchainImages.push_back(img);
		}

}

	
	void ResManager::clearSwapchain(VulkanContext &ctx, uint32_t imagesInFlight) {
		for (int i{}; i < imagesInFlight; i++) {
			ctx.device.destroyImageView(swapchainImages[i].view);
		}
		swapchainImages.clear();
		ctx.device.destroySwapchainKHR(ctx.swapchain);
	}

	void ResManager::rethinkSwapchain(VulkanContext &ctx, uint32_t width, uint32_t height, uint32_t imagesInFlight) {


		clearSwapchain(ctx,imagesInFlight);
		createSwapchain(ctx,width,height,imagesInFlight);


	}
	

