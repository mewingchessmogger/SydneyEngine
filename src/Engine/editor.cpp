#include "editor.hpp"
#include <iostream>
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


void Editor::updateEditorInput(){
	ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
}
void Editor::evalViewport(vk::Sampler sampler, std::vector<AllocatedImage>& inputTargets)
{

	std::array<vk::ImageView,2> targets = {inputTargets[0].view, inputTargets[1].view} ;
	//vk::Sampler sampler = stk.res.samplers[static_cast<int>(SamplerType::TEXTURE)];

	
	for(int i{}; i < 2; i++)
    if (targets[i] != cachedViews[i]) {
        
        if (cachedDescriptors[i] != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(cachedDescriptors[i]);
            cachedDescriptors[i] = VK_NULL_HANDLE;
        }

        // Allocate a stable new descriptor handle if the incoming target is valid
        if (targets[i]) {
            std::cout << "GOON!!";
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


void Editor::messingAround(vk::CommandBuffer buffer, ECS::Registry& reg, GameContext& ctx, uint32_t currentImgIndex, Camera& edtCam, PlatformGLFW& plt){
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    
    ImGui::NewFrame();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 screenPos = viewport->WorkPos;
    ImVec2 screenSize = viewport->WorkSize;

    float menuBarHeightPadding = 9.0f; // Increase this to make it taller
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(ImGui::GetStyle().FramePadding.x, menuBarHeightPadding));

    
    // 2. Set scale to what you want (e.g., 2.0x larger)
    ImGui::GetIO().FontGlobalScale = 2.0f;

    // 3. Render the menu bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::MenuItem("Save Scene")) { }
            if (ImGui::MenuItem("Load Scene")) { }
            if (ImGui::MenuItem("Create Scene")) { }
            ImGui::EndMenu();
        }
      
        ImGui::EndMainMenuBar();
    }

    // 4. Reset scale immediately
    ImGui::GetIO().FontGlobalScale = 1.5;

    // 2. IMPORTANT: Pop the style immediately so it doesn't affect other windows
    ImGui::PopStyleVar();

    ImVec2 editorPos = ImVec2(screenPos.x ,screenPos.y + 8.0f);
    ImVec2 editorSize = ImVec2(screenSize.x,screenSize.y- 8.0f);
        
    ImGui::SetNextWindowPos(editorPos);
    ImGui::SetNextWindowSize(editorSize);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    ImGuiDockNodeFlags dockFlags{};
   


    

    ImGui::Begin("SydneyEngine main", nullptr, flags | ImGuiWindowFlags_NoTitleBar);
     
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");    
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockFlags);
    ImGui::End();

    

    ImGui::Begin("Entities view",nullptr,flags | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Entities");
    ImGui::Separator();

	std::vector<int>& entities = reg.getLiveIDs();

	static int selectedEntity = -1;
	
	
	for(auto e : entities){

		bool isSelected = (e == selectedEntity);


		ImGui::PushID(e);

		std::string label = "Entity ID: " + std::to_string(e);
		if (ImGui::Selectable(label.c_str(), isSelected))
			selectedEntity = e;

		ImGui::PopID();
	}


	

    ImGui::End();
    ImGui::Begin("View Entity Components",nullptr,flags);

	if(selectedEntity != -1){
		std::string label = "Entity " + std::to_string(selectedEntity) + " Components" ;
		ImGui::Text(label.c_str());
		ImGui::Separator();

		// add components game/scene
		// review components

	}
    
    
    ImGui::End();
    
    

    updateCamera(edtCam,plt,0.3);


    ImGui::Begin("viewport",nullptr,flags);

    if(ImGui::IsMouseClicked(ImGuiMouseButton_Right)){
         plt.inputState.requestCursorVisible = !plt.inputState.requestCursorVisible;
         plt.updateState();
    }


    if (cachedDescriptors[currentImgIndex] != VK_NULL_HANDLE) {
            ImVec2 size = ImGui::GetContentRegionAvail();
            ImGui::Image((ImTextureID)(VkDescriptorSet)cachedDescriptors[currentImgIndex], size);
        }
    
    ImGui::End();

    
    ImGui::Begin("Files");

    
    ImGui::End();

    


    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);

}


void Editor::updateCamera(Camera& cam, PlatformGLFW& plt, float sens){
    
        float aspect = plt.aspectRatio;
		float dt = plt.deltaTime;

        
        float& pitch = cam.pitch;
        float& yaw = cam.yaw;
        auto& state = plt.inputState;
		pitch += state.mouseDY * 0.3;
		yaw += state.mouseDX * 0.3;

		if(pitch > 89.0f)
			pitch =  89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		

		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);

		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		glm::vec3 cameraFront = glm::normalize(direction);
		
		glm::vec3& eye = cam.eye;
		glm::vec3& cameraUp = cam.up;

	
		float sensitivity = 1.0f;

		float cameraYAxis{};
		
        cameraYAxis = cameraFront.y;
		



		if (state.keyHeld(Input::Key::Forward)){
			eye += glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Backward)){
			eye -= glm::vec3(cameraFront.x, cameraYAxis, cameraFront.z) * dt;
		}
		if (state.keyHeld(Input::Key::Left)){
			eye -= glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;

		}
		if (state.keyHeld(Input::Key::Right)){
			eye += glm::normalize(glm::cross(cameraFront, cameraUp)) * dt;
		}
		
		
		

		cam.view = glm::lookAt(eye, cameraFront+eye, cameraUp);
		cam.proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
		cam.dir = cameraFront;
}