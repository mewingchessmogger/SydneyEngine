
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

		// int width = 0, height = 0;
		// glfwGetFramebufferSize(wtx->window, &width, &height);
		// while (width == 0 || height == 0) {
		// 	glfwGetFramebufferSize(wtx->window, &width, &height);
		// 	glfwWaitEvents();
		// }
		ctx.device.waitIdle();
		clearSwapchain(ctx,imagesInFlight);
		createSwapchain(ctx,width,height,imagesInFlight);


	}


	bool ResManager::isValidSwapchain(VulkanContext &ctx, vk::ResultValue<uint32_t> imgResult, vk::Semaphore imageReadySemaphore, uint32_t width, uint32_t height, uint32_t imagesInFlight,bool &windowResized, int currFrame) {

		if (imgResult.result == vk::Result::eErrorOutOfDateKHR || windowResized == true) {
			windowResized = false;
			ctx.device.destroySemaphore(imageReadySemaphore);
			ctx.imageReadySemaphores[currFrame] = ctx.device.createSemaphore(vk::SemaphoreCreateInfo{});
			rethinkSwapchain(ctx,width,height,imagesInFlight);

			return false;
		}
		else if (imgResult.result != vk::Result::eSuccess && imgResult.result != vk::Result::eSuboptimalKHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		return true;
	}
