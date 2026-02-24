
#include "VkBootstrap.h"
#include "vk_stack.hpp"
#include "platform_glfw.hpp"
#include <vk_mem_alloc.h>
#include <iostream> 
void VulkanStack::initInstance(PlatformGLFW& plt) {

	vkb::InstanceBuilder instanceBuilder;
	uint32_t count{};
	const char** extensions = plt.getInstanceExtensions(&count);
	auto vkbInstanceBuilder = instanceBuilder.request_validation_layers(bUseValidationLayers)
		.use_default_debug_messenger()
		.require_api_version(1, 3, 0)
		.enable_extensions(count, extensions)
		.build();
	ctx.vkbInstance = new vkb::Instance(vkbInstanceBuilder.value());

	ctx.instance = vk::Instance{ ctx.vkbInstance->instance };
	ctx.debugMessenger = vk::DebugUtilsMessengerEXT{ ctx.vkbInstance->debug_messenger };
	
}


void VulkanStack::initDevice(PlatformGLFW& plt) {

    //necessary right here otherwise crash
    plt.createWindowSurface(ctx.instance,reinterpret_cast<VkSurfaceKHR*>(&ctx.surface));
    //use vkbootstrap to select a gpu
    //We want a gpu that can write to the SDL surface and supports vulkan 1.3 with the correct features
    vkb::PhysicalDeviceSelector physicalDeviceSelector(*ctx.vkbInstance);


    vk::PhysicalDeviceDescriptorIndexingFeatures descIndexFeatures{};
    vk::PhysicalDeviceVulkan13Features features13{};
    vk::PhysicalDeviceVulkan12Features features12{};
    vk::PhysicalDeviceVulkan11Features features11{};
    vk::PhysicalDeviceFeatures2 features2{};
    vk::PhysicalDeviceFeatures features1{};

    vk::PhysicalDeviceShaderDrawParameterFeatures shadFeat{};
    

    shadFeat.shaderDrawParameters = true;
    
    features13.dynamicRendering = true;
    features13.synchronization2 = true;
    
    //vulkan 1.2 features
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;
    features12.runtimeDescriptorArray = true;
    features12.shaderSampledImageArrayNonUniformIndexing = true;
    features12.descriptorBindingPartiallyBound = true;
    features12.descriptorBindingVariableDescriptorCount = true;
    features12.descriptorBindingSampledImageUpdateAfterBind = true;

    features1.samplerAnisotropy = true;
    features1.multiDrawIndirect = true;
    features1.textureCompressionBC = true;
    vkb::PhysicalDevice physicalDevice = physicalDeviceSelector
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_required_features(features1)
        .add_required_extension("VK_KHR_shader_draw_parameters")
        .add_required_extension_features(static_cast<VkPhysicalDeviceShaderDrawParameterFeatures>(shadFeat))
        .set_surface(ctx.surface)
        .select()
        .value();


    //create the final vulkan device
    vkb::DeviceBuilder deviceBuilder{ physicalDevice };

    ctx.vkbDevice = new vkb::Device{ deviceBuilder.build().value() };

    


    // Get the VkDevice handle used in the rest of a vulkan application
    ctx.device = vk::Device{ ctx.vkbDevice->device };

    auto resQueue = ctx.vkbDevice->get_queue(vkb::QueueType::graphics);
    auto resFamilyIndex = ctx.vkbDevice->get_queue_index(vkb::QueueType::graphics);

    ctx.graphicsQueue.handle = vk::Queue{ resQueue.value() };
    ctx.graphicsQueue.famIndex = resFamilyIndex.value();
    ctx.chosenGPU = vk::PhysicalDevice{ physicalDevice.physical_device };

    VmaAllocatorCreateInfo allocInfo{};
    allocInfo.physicalDevice = ctx.chosenGPU;
    allocInfo.device = ctx.device;
    allocInfo.instance = ctx.instance;
	allocInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
    vmaCreateAllocator(&allocInfo, &res.allocator);
    

    vk::PhysicalDeviceVulkan12Features supported12{};
    vk::PhysicalDeviceVulkan13Features supported13{};
    vk::PhysicalDeviceFeatures2 supported2{};

    supported2.pNext = &supported12;
    supported12.pNext = &supported13;

    ctx.chosenGPU.getFeatures2(&supported2);

    std::cout << "--- Vulkan Device Debugger ---\n";
    std::cout << "GPU Name: " << physicalDevice.properties.deviceName << "\n";
	
    std::cout << "Feature textureCompressionBC: " << (features1.textureCompressionBC ? "Enabled" : "Disabled") << "\n";
    std::cout << "Feature dynamicRendering: " << (supported13.dynamicRendering ? "Supported" : "Not Supported") << "\n";
    std::cout << "Feature synchronization2: " << (supported13.synchronization2 ? "Supported" : "Not Supported") << "\n";
    std::cout << "Feature bufferDeviceAddress: " << (supported12.bufferDeviceAddress ? "Supported" : "Not Supported") << "\n";
    std::cout << "GPU supports descriptorIndexing: "
        << supported12.descriptorIndexing << "\n";
    std::cout << "GPU supports runtimeDescriptorArray: "
        << supported12.runtimeDescriptorArray << "\n";
    std::cout << "GPU supports shaderSampledImageArrayNonUniformIndexing: "
        << supported12.shaderSampledImageArrayNonUniformIndexing << "\n";
    std::cout << "Extension VK_KHR_shader_draw_parameters: Enabled\n";
	std::cout << "min SSBO offset alignment: " << physicalDevice.properties.limits.minStorageBufferOffsetAlignment << "\n";
    std::cout << "------------------------------\n";

}


void VulkanStack::createSwapchain() {

	}

    

void VulkanStack::initCommands() {

	vk::CommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.setQueueFamilyIndex(ctx.graphicsQueue.famIndex)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	ctx.cmdPool = ctx.device.createCommandPool(cmdPoolInfo);
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setCommandPool(ctx.cmdPool)
		.setCommandBufferCount(NUM_OF_IMAGES)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	ctx.cmdBuffers.resize(NUM_OF_IMAGES);
	ctx.cmdBuffers = ctx.device.allocateCommandBuffers(allocInfo);
	for (auto i : ctx.cmdBuffers) {
		std::cout <<"cmdbuffer #"<< i << "\n";
	}
	res.delQ.add(ctx.cmdPool);	
 
 //imgui stuff
	
	vk::CommandPoolCreateInfo immPoolInfo{};
	immPoolInfo.setQueueFamilyIndex(ctx.graphicsQueue.famIndex)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	ctx.immPool = ctx.device.createCommandPool(immPoolInfo);
	allocInfo.setCommandPool(ctx.immPool)
		.setCommandBufferCount(NUM_OF_IMAGES)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	ctx.immBuffers.resize(NUM_OF_IMAGES);
	ctx.immBuffers = ctx.device.allocateCommandBuffers(allocInfo);
	for (auto i : ctx.immBuffers) {
		std::cout << "immgui cmdBuffer#" << i << "\n";
	}
	res.delQ.add(ctx.immPool);
		
}




void VulkanStack::initSyncs() {

	vk::FenceCreateInfo createInfo{};
	createInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	vk::SemaphoreCreateInfo semaphInfo{};

	for (int i{}; i < 2; i++) {
		auto imgR =ctx.device.createSemaphore(semaphInfo);
		auto renF = ctx.device.createSemaphore(semaphInfo);
		auto fence = ctx.device.createFence(createInfo);
		auto immFence = ctx.device.createFence(createInfo);
		ctx.imageReadySemaphores[i] = imgR;
		ctx.renderFinishedSemaphores[i] = renF;
		ctx.fences.push_back(fence);
		ctx.immFences.push_back(immFence);
		
		res.delQ.add(imgR);
		res.delQ.add(renF);
		res.delQ.add(fence);
		res.delQ.add(immFence);

	}
	


}
void VulkanStack::initDescriptorStuff(){
	res.initDescriptorPoolAndSets(ctx.device, res.MAX_IMAGES, res.MAX_SAMPLER);
	res.initAndUpdateSamplers(ctx.device,ctx.chosenGPU.getProperties().limits.maxSamplerAnisotropy);
	//res.delQ.add() <--------------------------------------/////////////////////////////////////////////////////////////////// add later
}

void VulkanStack::initBuffers(){
	res.initBuffers(ctx.device);
}


void VulkanStack::initSwapchain(){
	
	res.createSwapchain(ctx,WIDTH,HEIGHT,NUM_OF_IMAGES);
	
}