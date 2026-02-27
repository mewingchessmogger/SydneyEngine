
#include "pipeline_builder.hpp"





PipelineBuilder& PipelineBuilder::setDynRendering(int expectedClrAttachmentsCount, std::vector<vk::Format> formatClrAttachments, vk::Format formatOfDepth) {
	
	clrFormats = formatClrAttachments;
	depthFormat = formatOfDepth;
	dynRenderInfo.setDepthAttachmentFormat(depthFormat);

	if (clrFormats.size() == 0) {
		dynRenderInfo
			.setColorAttachmentCount(0)
			.setPColorAttachmentFormats(nullptr);
		return *this;
	}

	dynRenderInfo
		.setColorAttachmentCount(expectedClrAttachmentsCount)
		.setPColorAttachmentFormats(clrFormats.data());
	return *this;
}


PipelineBuilder& PipelineBuilder::setShaderStages(const std::vector<uint32_t> &vertCompiled, const std::vector<uint32_t> &fragCompiled,bool isDepthPass) {


    vk::ShaderModuleCreateInfo vertInfo{};
    vertInfo.setCode(vertCompiled);
    vk::ShaderModuleCreateInfo fragInfo{};
    fragInfo.setCode(fragCompiled);

	vertShader = device.createShaderModule(vertInfo);
    fragShader = device.createShaderModule(fragInfo);

    vk::PipelineShaderStageCreateInfo vertexPipelineInfo{};
	vertexPipelineInfo
		.setStage(vk::ShaderStageFlagBits::eVertex)
		.setModule(vertShader)
		.setPName("main");

	if (isDepthPass) {
		shaderStages = { vertexPipelineInfo };
		return *this;
	}
    
	vk::PipelineShaderStageCreateInfo fragmentPipelineInfo{};
	fragmentPipelineInfo
		.setStage(vk::ShaderStageFlagBits::eFragment)
		.setModule(fragShader)
		.setPName("main");

	shaderStages = { vertexPipelineInfo,fragmentPipelineInfo };
	
	return *this;
}

PipelineBuilder& PipelineBuilder::setVertexInputInfo()
{
	//were using bda no need here fo rnaything
		vertexInputInfo
        .setVertexBindingDescriptionCount(0)
        .setPVertexBindingDescriptions(nullptr) 
        .setVertexAttributeDescriptionCount(0)
		.setPVertexAttributeDescriptions(nullptr);
		return *this;
	
	return *this;
}

PipelineBuilder& PipelineBuilder::setAssemblyInfo(vk::PrimitiveTopology top) {
	assInfo.setTopology(top); 
	return *this;
}

PipelineBuilder& PipelineBuilder::setScissorAndViewport(vk::Extent2D ext)
{

	scissor
		.setOffset(vk::Offset2D{ 0,0 })
		.setExtent(ext);

	viewport
		.setX(0)
		.setY(0)
		.setWidth((float)ext.width)
		.setHeight((float)ext.height)
		.setMinDepth(0.0)
		.setMaxDepth(1.0);
	viewportInfo
		.setScissorCount(1)
		.setViewportCount(1);

	return *this;
}

PipelineBuilder& PipelineBuilder::setRasterizerInfo(vk::PolygonMode poly, vk::FrontFace frontFace, bool isDepthPass)
{

	razInfo
		.setDepthClampEnable(vk::False)
		.setRasterizerDiscardEnable(vk::False)
		.setPolygonMode(poly)

		.setCullMode(vk::CullModeFlagBits::eBack)
		
		.setFrontFace(frontFace)
		.setDepthBiasEnable(vk::True)
		.setDepthBiasSlopeFactor(1.0)
		.setLineWidth(1.0);
	return *this;
}

PipelineBuilder& PipelineBuilder::setMultiSampling()
{

	multisampleInfo
		.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setSampleShadingEnable(vk::False);
	return *this;
}

PipelineBuilder& PipelineBuilder::setBlendState(bool isDepthPass)
{


	if (isDepthPass) {
		blendInfo.setAttachmentCount(0).setPAttachments(nullptr);
		return *this;
	}

	attachment.colorWriteMask =
		vk::ColorComponentFlagBits::eR |
		vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eB |
		vk::ColorComponentFlagBits::eA;
	attachment.blendEnable = vk::True;
	attachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
	attachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
	attachment.colorBlendOp = vk::BlendOp::eAdd;
	attachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
	attachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
	attachment.alphaBlendOp = vk::BlendOp::eAdd;
	blendInfo.setAttachments(attachment);
	return *this;
}

PipelineBuilder& PipelineBuilder::setDynState()
{
	dynamicState.setDynamicStates(dynamicStates);
	return *this;
}

PipelineBuilder& PipelineBuilder::setPCRange(size_t sizePC, int offset, vk::ShaderStageFlags stage)
{
	pcRange
		.setOffset(offset)
		.setSize(sizePC)
		.setStageFlags(stage);

	layoutInfo.setPushConstantRanges(pcRange);
	return *this;
}

PipelineBuilder& PipelineBuilder::setDescLayout(std::vector<vk::DescriptorSetLayout> descLayouts)
{
	descriptorSetLayouts = descLayouts;
	layoutInfo.setSetLayouts(descriptorSetLayouts);
	return *this;
}

PipelineBuilder& PipelineBuilder::createPipeLineLayout()
{
	
	pipeLineLayout = device.createPipelineLayout(layoutInfo);
	return *this;
}

PipelineBuilder& PipelineBuilder::setDepthStencilState(vk::Bool32 depthTestEnable, vk::Bool32 depthWriteEnable)
{
	depthInfo
		.setDepthTestEnable(depthTestEnable)
		.setDepthWriteEnable(depthWriteEnable)
		.setDepthCompareOp(vk::CompareOp::eLessOrEqual)//if the incoming depth is "less" than stored depth we store the incoming fragment otherwise discard
		.setDepthBoundsTestEnable(vk::False)//some obscure thing
		.setStencilTestEnable(vk::False);//what is stencils duhguh
	//.setFront() //these two need to be set if stenciltest is enabled!
	//.setBack()
	return *this;
}

PipelineBuilder& PipelineBuilder::createPipeline()
{
	createPipelineInfo
		.setPNext(&dynRenderInfo)			// setting or dynrendering
		.setFlags(vk::PipelineCreateFlags{})// tweakiung pipelines set to nothing
		.setStages(shaderStages)// ctx->arr of shaderstageInfos
		.setPVertexInputState(&vertexInputInfo)// setting up buffers future
		.setPInputAssemblyState(&assInfo)// setting up that interpereting every three oints as a triangle here
		.setPViewportState(&viewportInfo)
		.setPRasterizationState(&razInfo)
		.setPMultisampleState(&multisampleInfo)
		.setPDepthStencilState(&depthInfo)
		.setPColorBlendState(&blendInfo)
		.setPDynamicState(&dynamicState)
		.setLayout(pipeLineLayout)
		.setBasePipelineHandle(vk::Pipeline{})
		.setBasePipelineIndex(-1);


	pipeline=device.createGraphicsPipeline({}, createPipelineInfo).value;
	
	
	device.destroyShaderModule(vertShader);
	device.destroyShaderModule(fragShader);


	pipelineCreated = true;

	

	
	return *this;
}

vk::Pipeline PipelineBuilder::getPipeline() {

	if (!pipelineCreated) {
		throw std::runtime_error("pipeline not created");
	}

	
	return pipeline;
}
void PipelineBuilder::resetBuild() {
	clrFormats.clear();
	depthFormat = vk::Format::eUndefined;

	bindings.clear();
	attribs.clear();
	vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{};

	assInfo = vk::PipelineInputAssemblyStateCreateInfo{};

	viewportInfo = vk::PipelineViewportStateCreateInfo{};
	scissor = vk::Rect2D{};
	dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	dynamicState = vk::PipelineDynamicStateCreateInfo{}.setDynamicStates(dynamicStates);

	razInfo = vk::PipelineRasterizationStateCreateInfo{};
	multisampleInfo = vk::PipelineMultisampleStateCreateInfo{};
	depthInfo = vk::PipelineDepthStencilStateCreateInfo{};
	stencilInfo = vk::PipelineDepthStencilStateCreateInfo{};

	attachment = vk::PipelineColorBlendAttachmentState{};
	blendInfo = vk::PipelineColorBlendStateCreateInfo{};

	shaderStages.clear();
	vertShader = nullptr;
	fragShader = nullptr;

	pcRange = vk::PushConstantRange{};
	descriptorSetLayouts.clear();
	layoutInfo = vk::PipelineLayoutCreateInfo{};

	dynRenderInfo = vk::PipelineRenderingCreateInfo{};
	createPipelineInfo = vk::GraphicsPipelineCreateInfo{};
	pipeline = nullptr;

	pipelineCreated = false;
	pipeLineLayout = vk::PipelineLayout{};
	viewport = vk::Viewport{};

}
