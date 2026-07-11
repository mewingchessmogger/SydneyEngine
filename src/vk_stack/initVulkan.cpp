
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
    vkb::PhysicalDeviceSelector physicalDeviceSelector(*ctx.vkbInstance);


    vk::PhysicalDeviceDescriptorIndexingFeatures descIndexFeatures{};
    vk::PhysicalDeviceVulkan13Features features13{};
    vk::PhysicalDeviceVulkan12Features features12{};
    vk::PhysicalDeviceVulkan11Features features11{};
    vk::PhysicalDeviceFeatures2 features2{};
    vk::PhysicalDeviceFeatures features1{};

    vk::PhysicalDeviceShaderDrawParameterFeatures shadFeat{};
    VkPhysicalDeviceMaintenance6FeaturesKHR feat6{};

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
    features1.shaderInt64 = true;
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
    std::cout << "min UBO offset alignment: " << physicalDevice.properties.limits.minUniformBufferOffsetAlignment << "\n";
    std::cout << "------------------------------\n";
     std::cout << "------------------------------\n";
    std::cout << "max push constant size: " << physicalDevice.properties.limits.maxPushConstantsSize << "\n";
    std::cout << "------------------------------\n";
}


void VulkanStack::initCommands() {

	vk::CommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.setQueueFamilyIndex(ctx.graphicsQueue.famIndex)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	ctx.cmdPool = ctx.device.createCommandPool(cmdPoolInfo);
	vk::CommandBufferAllocateInfo allocInfo{};
	allocInfo.setCommandPool(ctx.cmdPool)
		.setCommandBufferCount(DESIRED_IMAGES_IN_FLIGHT)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	cmdBuffers.resize(DESIRED_IMAGES_IN_FLIGHT);
	cmdBuffers = ctx.device.allocateCommandBuffers(allocInfo);
	for (auto i : cmdBuffers) {
		std::cout <<"cmdbuffer #"<< i << "\n";
	}
	res.delQ.add(ctx.cmdPool);	
 
 //imgui stuff
	
	vk::CommandPoolCreateInfo immPoolInfo{};
	immPoolInfo.setQueueFamilyIndex(ctx.graphicsQueue.famIndex)
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
	ctx.immPool = ctx.device.createCommandPool(immPoolInfo);
	allocInfo.setCommandPool(ctx.immPool)
		.setCommandBufferCount(DESIRED_IMAGES_IN_FLIGHT)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	ctx.immBuffers.resize(DESIRED_IMAGES_IN_FLIGHT);
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
	//res.delQ.add() <--------------------------------------/////////////////////////////////////////////////////////////////// add later
}

void VulkanStack::initUpdateDescriptorSets(){
	res.initAndUpdateSamplers(ctx.device,ctx.chosenGPU.getProperties().limits.maxSamplerAnisotropy);

    vk::DescriptorBufferInfo uboBufferInfo{};
    uboBufferInfo
        .setBuffer(res.uniformBuffer.handle)
        .setOffset(0)            // Dynamic UBOs start at 0; offset is provided at bind time
        .setRange(res.uniformBuffer.stride);  // IMPORTANT: The size of ONE frame's data

    vk::WriteDescriptorSet uboWrite{};
            std::cout << "!!!!" << res.descriptorSets[static_cast<size_t>(DescriptorSetType::UBO)] << "\n";

    uboWrite
        .setDstSet(res.descriptorSets[static_cast<size_t>(DescriptorSetType::UBO)])
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
        .setBufferInfo(uboBufferInfo);

    
    vk::DescriptorImageInfo viewportInfo{};
    viewportInfo
        .setImageView(res.viewportImages[0].view)
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::DescriptorImageInfo secviewportInfo{};    
    secviewportInfo
        .setImageView(res.viewportImages[1].view)
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    vk::DescriptorImageInfo colorPickInfo{};
    colorPickInfo
        .setImageView(res.colorPickImage.view)
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);

    // Build the structural array configuration matching your consecutive bindless slot layouts
    std::array<vk::DescriptorImageInfo, 3> bindlessImages = {
        viewportInfo,    // Slot index 0 -> Main Viewport Color Target
        secviewportInfo, // Slot index 1 -> Alternate Viewport Frame / Target 2
        colorPickInfo        // Slot index 2 -> GPU Color Picking Registry Image
    };

    vk::WriteDescriptorSet bindlessWrite{};
    bindlessWrite
        .setDstSet(res.descriptorSets[static_cast<size_t>(DescriptorSetType::IMAGE)]) // Targeting Set 1
        .setDstBinding(0)                                                           // Target Layout Binding 0
        .setDstArrayElement(0)                                                      // Start writing at array index slot 0
        .setDescriptorType(vk::DescriptorType::eSampledImage)                       // Explicit non-combined sampled specifier
        .setDescriptorCount(static_cast<uint32_t>(bindlessImages.size()))           // Update all 3 slots simultaneously
        .setPImageInfo(bindlessImages.data());

    // 3. Complete Pipeline Updates Synchronously
    std::array<vk::WriteDescriptorSet, 2> pipelineWrites = { uboWrite, bindlessWrite };
    ctx.device.updateDescriptorSets(pipelineWrites, nullptr);
}


void VulkanStack::initDepthImages(){

    
    AllocatedImage depth{};
    AllocatedImage depth2{};
    res.requestImage(ctx.device,ImgType::DEPTH, depth, SET_WIDTH,SET_HEIGHT);
    res.requestImage(ctx.device,ImgType::DEPTH, depth2, SET_WIDTH,SET_HEIGHT);
    res.zBufferImages.push_back(depth);
    res.zBufferImages.push_back(depth2);
    
}

void VulkanStack::initColorPickImage(){
    res.requestImage(ctx.device,ImgType::CLR_PICKING, res.colorPickImage, SET_WIDTH,SET_HEIGHT);
}



void VulkanStack::initBuffers(){
	res.initBuffers(ctx.device,ctx.chosenGPU.getProperties().limits.minUniformBufferOffsetAlignment,DESIRED_IMAGES_IN_FLIGHT);
    
}


void VulkanStack::initSwapchain(){
	
	res.createSwapchain(ctx,WIDTH,HEIGHT,DESIRED_IMAGES_IN_FLIGHT);
	
}

void VulkanStack::initRenderTargetImages()
{
     
    AllocatedImage target{};
    AllocatedImage target2{};
    
    res.requestImage(ctx.device,ImgType::RENDER_TARGET, target, SET_WIDTH,SET_HEIGHT);
    res.requestImage(ctx.device,ImgType::RENDER_TARGET, target2, SET_WIDTH,SET_HEIGHT);
    res.renderTargetImages.push_back(target);
    res.renderTargetImages.push_back(target2);
    
}


void VulkanStack::initViewportImages()
{
     
    AllocatedImage target{};
    AllocatedImage target2{};
    
    res.requestImage(ctx.device,ImgType::TEXTURE, target, SET_WIDTH,SET_HEIGHT);
    res.requestImage(ctx.device,ImgType::TEXTURE, target2, SET_WIDTH,SET_HEIGHT);
    res.viewportImages.push_back(target);
    res.viewportImages.push_back(target2);
    
    ImmediateTransitionViewport();

}

void VulkanStack::ImmediateTransitionViewport(){
    // 1. Grab your raw graphics queue and command buffer
    // (Assuming currentFrame is 0 or you just want to use the first buffer)
    vk::CommandBuffer cmdBuffer = cmdBuffers[currentFrame];

    // 2. Begin recording on the command buffer
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmdBuffer.begin(beginInfo);

    // 3. Record your actual layout changes
    transitionImage(res.viewportImages[0], vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
    transitionImage(res.viewportImages[1], vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);

    // 4. End recording
    cmdBuffer.end();

    // 5. Submit directly to the GPU queue
    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;
    ctx.graphicsQueue.handle.submit(submitInfo, nullptr); // No fence needed because we waitIdle right after
    // 6. Block the CPU until the GPU executes the transitions
    ctx.device.waitIdle();
    
    // 7. Reset the command buffer so it's fresh and ready for your normal frame loop
    cmdBuffer.reset();

}