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

#include <imgui_internal.h> // Required for DockBuilder API access

void Editor::render(vk::CommandBuffer buffer, ECS::Registry& reg, GameContext& ctx, uint32_t currentFrame)
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Get current GLFW window footprint
    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(mainViewport->WorkPos);
    ImGui::SetNextWindowSize(mainViewport->WorkSize);
    ImGui::SetNextWindowViewport(mainViewport->ID);

    // Lock the background canvas completely flat
    ImGuiWindowFlags hostFlags = 
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | 
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | 
        ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("MasterCanvas", nullptr, hostFlags);
    ImGui::PopStyleVar(3);

        ImGuiID dockspaceId = ImGui::GetID("MainAppDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        
        // ----------------------------------------------------------------
        // FIRST RUN INITIALIZER: BUILD THE DEFAULT ENGINE SPLITS
        // ----------------------------------------------------------------
        if (isFirstFrame) {
            isFirstFrame = false;

            // Clear any old layout memory cache strings
            ImGui::DockBuilderRemoveNode(dockspaceId); 
            ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceId, mainViewport->WorkSize);

            ImGuiID leftNodeId;
            ImGuiID rightNodeId;
            ImGuiID topRightNodeId;
            ImGuiID bottomRightNodeId;

            // Split 1: Split the screen horizontally (70% Left for Game, 30% Right for tools)
            ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.70f, &leftNodeId, &rightNodeId);

            // Split 2: Take the right tools column and split it vertically (50% Top, 50% Bottom)
            ImGui::DockBuilderSplit(rightNodeId, ImGuiDir_Up, 0.50f, &topRightNodeId, &bottomRightNodeId);

            // Fasten your individual window names to the generated layout slots permanently
            ImGui::DockBuilderDockWindow("Game Viewport 0", leftNodeId);
            ImGui::DockBuilderDockWindow("Scene Hierarchy", topRightNodeId);
            ImGui::DockBuilderDockWindow("Entity Inspector", bottomRightNodeId);

            ImGui::DockBuilderFinish(dockspaceId);
        }
    
    ImGui::End(); // Close MasterCanvas Container

    // ----------------------------------------------------------------
    // THE WINDOWS (Zero constraints, zero lock flags! Clean and resizable)
    // ----------------------------------------------------------------
    
    // Left Pane
    ImGui::Begin("Game Viewport 0");
        if (cachedDescriptors[currentFrame] != VK_NULL_HANDLE) {
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImGui::Image((ImTextureID)(VkDescriptorSet)cachedDescriptors[currentFrame], size);
        }
    ImGui::End();

    // Top Right Pane
    ImGui::Begin("Scene Hierarchy");
        ImGui::Separator();
        auto& IDs = reg.getLiveIDs();
        for (auto e : IDs){
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Leaf;
            std::string label = "Entity ID: " + std::to_string(e);
            bool opened = ImGui::TreeNodeEx((void*)(uint64_t)e, flags, "%s", label.c_str());
            if (ImGui::IsItemClicked()) { selectedEntity = e; }
            if (opened) { ImGui::TreePop(); }
        }
    ImGui::End();

    // Bottom Right Pane
    ImGui::Begin("Entity Inspector");
        if(ImGui::Button("Save Game to File")){
            std::cout <<" SAVING !!...\n";
        }
        ImGui::Separator();

        if (selectedEntity == -1) {
            ImGui::Text("Select an entity.");
        } 
        else {
            ImGui::Text("Editing Entity ID: %d", selectedEntity);   
            for (auto& [key, pool] : reg.getPoolMap()){
                if (!pool->hasEntity(selectedEntity)) continue; 
                
                auto fields = pool->getComponentFields(selectedEntity);
                for (const auto& field : fields) {
                    std::string label = std::string(field.name);
                    switch (field.ptr.index()) {
                        case 0: {
                            glm::vec3* val = std::get<0>(field.ptr);
                            ImGui::DragFloat3(label.c_str(), &val->x, 0.1f);
                            break;
                        }
                        case 1: {
                            int* val = std::get<1>(field.ptr);
                            ImGui::DragInt(label.c_str(), val, 1);
                            break;
                        }
                        case 2: {
                            float* val = std::get<2>(field.ptr);
                            ImGui::DragFloat(label.c_str(), val, 0.1f);
                            break;
                        }
                        case 3: {
                            uint32_t* val = std::get<3>(field.ptr);
                            int tempVal = static_cast<int>(*val);
                            if (ImGui::DragInt(label.c_str(), &tempVal, 1, 0, 99999)) {
                                *val = static_cast<uint32_t>(tempVal);
                            }
                            break;
                        }
                    }
                }
            }
        }
    ImGui::End();

    // Final frame execution steps
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
}
void Editor::updateEditorInput(){
	ImGui::UpdatePlatformWindows();
}
void Editor::evalViewport(vk::Sampler sampler, std::vector<AllocatedImage>& inputTargets)
{

	std::array<vk::ImageView,2> targets = {inputTargets[0].view, inputTargets[1].view} ;
	//vk::Sampler sampler = stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)];

	
	for (int i {}; i < 2; i++){
		if (targets[i] != cachedViews[i]) {
            
            if (cachedDescriptors[i] != VK_NULL_HANDLE) {
                ImGui_ImplVulkan_RemoveTexture(cachedDescriptors[i]);
                cachedDescriptors[i] = VK_NULL_HANDLE;
            }

            // Allocate a stable new descriptor handle if the incoming target is valid
            if (targets[i]) {
                cachedDescriptors[i] = ImGui_ImplVulkan_AddTexture(
                    (VkSampler)sampler, 
                    (VkImageView)targets[i], 
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                );
            }
            
            // Update the static tracking cache pointer
            cachedViews[i] = targets[i];
        }
	}

}
