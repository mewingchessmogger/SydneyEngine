#include "vk_stack.hpp"
void VulkanStack::initTestPipeline(std::vector<uint32_t>&& vertSpv, std::vector<uint32_t>&& fragSpv){
    plb
		.setDevice(ctx.device)
		.setDynRendering(1, { res.swapchainImages[0].format })
		.setShaderStages(vertSpv,fragSpv,false)
		.setVertexInputInfo()
		.setAssemblyInfo()
		.setScissorAndViewport(res.swapchainImages[0].extent2D)
		.setRasterizerInfo(vk::PolygonMode::eFill, vk::FrontFace::eCounterClockwise)
		.setMultiSampling()//nuffin right now
		.setBlendState()
		.setDynState()
		.setPCRange(sizeof(0), 0, vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
		.setDescLayout({})
		.createPipeLineLayout()
		.setDepthStencilState()
		.createPipeline();

    
    testPSO.handle = plb.getPipeline();
    testPSO.layout = plb.pipeLineLayout;
	plb.resetBuild();
}