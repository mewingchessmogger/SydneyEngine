#include "vk_stack.hpp"
#include "buffer_containers.hpp"
void VulkanStack::initTestPipeline(std::vector<uint32_t>&& vertSpv, std::vector<uint32_t>&& fragSpv){

	// CreatePipelineInfo pInfo{};
	// pInfo
	// 	.setClrAttachCount(1)
	// 	.setClrFormat(res.swapchainImages[0].format)
	// 	.setDepthFormat(vk::Format::eUndefined)
	// 	.setIsDepthPass(false)
	// 	.setTopology(vk::PrimitiveTopology::eTriangleList)
	// 	.setExtent(res.swapchainImages[0].extent2D)
	// 	.setPolyMode(vk::PolygonMode::eFill)
	// 	.setFrontFace(vk::FrontFace::eCounterClockwise)
	// 	.setBytesPC(0)
	// 	.setStagePC(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
	// 	.setDepthWrite(vk::True)
	// 	.setDepthTest(vk::True)
	// 	.setDescSetLayout({res.descriptorSetLayouts[0], res.descriptorSetLayouts[1],res.descriptorSetLayouts[2]});
	// 	testPSO = createPipeline(vertSpv,fragSpv,pInfo); 
	
}
void VulkanStack::initPhongPipeline(std::vector<uint32_t>&& vertSpv, std::vector<uint32_t>&& fragSpv){
	
	

	CreatePipelineInfo pInfo{};
	
	pInfo
		.setClrAttachCount(1)
		.setClrFormat(res.swapchainImages[0].format)
		.setDepthFormat(vk::Format::eD32Sfloat)
		.setIsDepthPass(false)
		.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setExtent(res.swapchainImages[0].extent2D)
		.setPolyMode(vk::PolygonMode::eFill)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setBytesPC(sizeof(PushC::Model))
		.setStagePC(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
		.setDepthWrite(vk::True)
		.setDepthTest(vk::True)
		.setBlendState(false, true)
		.setDescSetLayout({res.descriptorSetLayouts[2]});//res.descriptorSetLayouts[0], res.descriptorSetLayouts[1],
		
		pipelines.phong = createPipeline(vertSpv,fragSpv,pInfo);
		
}

// void VulkanStack::initPickPipeline(std::vector<uint32_t>&& vertSpv, std::vector<uint32_t>&& fragSpv){
	
	

// 	CreatePipelineInfo pInfo{};
	
// 	pInfo
// 		.setClrAttachCount(1)
// 		.setClrFormat(res.colorPickImage.format)
// 		.setDepthFormat(vk::Format::eD32Sfloat)
// 		.setIsDepthPass(false)
// 		.setTopology(vk::PrimitiveTopology::eTriangleList)
// 		.setExtent(res.colorPickImage.extent2D)
// 		.setPolyMode(vk::PolygonMode::eFill)
// 		.setFrontFace(vk::FrontFace::eCounterClockwise)
// 		.setBytesPC(sizeof(PushC::Model))
// 		.setBlendState(false, false)
// 		.setStagePC(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
// 		.setDepthWrite(vk::True)
// 		.setDepthTest(vk::True)
// 		.setDescSetLayout({res.descriptorSetLayouts[2]});//res.descriptorSetLayouts[0], res.descriptorSetLayouts[1],

// 		PickPSO = createPipeline(vertSpv,fragSpv,pInfo);

// }


PipelineBundle VulkanStack::createPipeline(const std::vector<uint32_t>& vertSpv, const std::vector<uint32_t>& fragSpv,CreatePipelineInfo p){

	 plb
		.setDevice(ctx.device)
		.setDynRendering(p.clrAttachCount, {p.clrFormat },p.depthFormat)
		.setShaderStages(vertSpv,fragSpv,p.isDepthPass)
		.setVertexInputInfo()
		.setAssemblyInfo(p.top)
		.setScissorAndViewport(p.viewportExtent)
		.setRasterizerInfo(p.polyMode,p.fFace)
		.setMultiSampling()//nuffin right now
		.setBlendState(p.isDepthPass, p.blendEnable)
		.setDynState()
		.setPCRange(p.bytesPC, 0, p.stagePC)
		.setDescLayout(p.descSetLayouts)
		.createPipeLineLayout()
		.setDepthStencilState(p.depthTestEnable,p.depthWriteEnable)
		.createPipeline();
	

	PipelineBundle pso{};
	pso.handle = plb.getPipeline();
	pso.layout =  plb.pipeLineLayout;
    plb.resetBuild();
	return pso;
}