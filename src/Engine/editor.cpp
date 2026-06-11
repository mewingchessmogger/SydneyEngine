#include "editor.hpp"

void Editor::init(VulkanStack& stk, PlatformGLFW& plt)
{
	// 1. Descriptor pool creation using Vulkan-Hpp
	std::array<vk::DescriptorPoolSize, 11> poolSizes = {
		vk::DescriptorPoolSize{ vk::DescriptorType::eSampler,                 1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler,    1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage,            1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage,            1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer,      1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer,      1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer,           1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer,           1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic,    1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic,    1000 },
		vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment,         1000 }
	};

	vk::DescriptorPoolCreateInfo poolInfo{};
	poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	poolInfo.maxSets = 1000;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
    
	pool = stk.ctx.device.createDescriptorPool(poolInfo);

	IMGUI_CHECKVERSION();
	edtContext = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Setup Platform/Renderer backends
    
	ImGui_ImplGlfw_InitForVulkan(plt.windowPtr,true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = stk.ctx.instance;
	init_info.PhysicalDevice = stk.ctx.chosenGPU;
	init_info.Device = stk.ctx.device;
	init_info.QueueFamily = stk.ctx.graphicsQueue.famIndex;
	init_info.Queue = stk.ctx.graphicsQueue.handle;
	init_info.PipelineCache = VK_NULL_HANDLE;
	init_info.DescriptorPool = pool;
	init_info.MinImageCount = (uint32_t)stk.res.swapchainImages.size();
	init_info.ImageCount = (uint32_t)stk.res.swapchainImages.size();
	init_info.Allocator = nullptr;
	init_info.UseDynamicRendering = true;

	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = nullptr;
	static VkPipelineRenderingCreateInfoKHR renderingInfo{};
	renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
	renderingInfo.pNext = nullptr;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
    
	VkFormat rawSwapchainFormat = static_cast<VkFormat>(stk.res.swapchainImages[0].format);
	renderingInfo.pColorAttachmentFormats = &rawSwapchainFormat;

	renderingInfo.depthAttachmentFormat = VK_FORMAT_D32_SFLOAT;
	renderingInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;

	init_info.PipelineInfoMain.PipelineRenderingCreateInfo = renderingInfo;
	ImGui_ImplVulkan_Init(&init_info);



}



void Editor::render(vk::CommandBuffer buffer, ECS::Registry& reg, GameContext& ctx, bool showEditor){
	
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	
    if (showEditor){
		ImGui::Begin("Debug Window"); // <--- Must add this
		ImGui::Text("Debug View");
		ImGui::End();
	}

	ImGui::Render();

	if (showEditor){
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
	}

	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();


}

void Editor::buildUI()
{

}
