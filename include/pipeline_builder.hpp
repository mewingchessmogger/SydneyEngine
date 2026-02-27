#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
class PipelineBuilder {
	
public:

	
	PipelineBuilder& setDevice(vk::Device dev) {
		device = dev;
		return *this;
	}
PipelineBuilder& PipelineBuilder::setDynRendering(int expectedClrAttachmentsCount, std::vector<vk::Format> formatClrAttachments, vk::Format formatOfDepth = vk::Format::eUndefined);	
    PipelineBuilder &setShaderStages(const std::vector<uint32_t> &vertCompiled, const std::vector<uint32_t> &fragCompiled, bool isDepthPass);
    PipelineBuilder &setVertexInputInfo();
    PipelineBuilder& setAssemblyInfo(vk::PrimitiveTopology top = vk::PrimitiveTopology::eTriangleList);
	PipelineBuilder& setScissorAndViewport(vk::Extent2D ext);

	PipelineBuilder& setRasterizerInfo(vk::PolygonMode poly = vk::PolygonMode::eFill, vk::FrontFace frontFace = vk::FrontFace::eClockwise,bool isDepthPass = false);
	PipelineBuilder& setMultiSampling();
	PipelineBuilder& setBlendState(bool isDepthPass = false);
	PipelineBuilder& setDynState();
	PipelineBuilder& setPCRange(size_t sizePC, int offset = 0, vk::ShaderStageFlags stage = vk::ShaderStageFlagBits::eVertex);
	PipelineBuilder& setDescLayout(std::vector<vk::DescriptorSetLayout> descLayouts);
	PipelineBuilder& createPipeLineLayout();
	PipelineBuilder& setDepthStencilState(vk::Bool32 depthTestEnable = vk::True, vk::Bool32 depthWriteEnable = vk::True);
	PipelineBuilder& createPipeline();
	vk::Pipeline getPipeline();
	void resetBuild();
	vk::PipelineLayout pipeLineLayout;
	vk::Viewport viewport{};
	
private:
	vk::Device device;
	std::vector<vk::Format> clrFormats{};
	vk::Format depthFormat{};

	std::vector<vk::VertexInputBindingDescription> bindings = {};
	std::vector<vk::VertexInputAttributeDescription> attribs = {};

	bool pipelineCreated{ false };
	
	vk::PipelineRenderingCreateInfo dynRenderInfo{};
	
	
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{};
	vk::ShaderModule vertShader{};
	vk::ShaderModule fragShader{};


	//setvertexinput
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
	
	//setassembly
	vk::PipelineInputAssemblyStateCreateInfo assInfo{};


	//setscissor
	vk::PipelineViewportStateCreateInfo viewportInfo{};
	vk::Rect2D scissor{};

	vk::PipelineRasterizationStateCreateInfo razInfo{};

	vk::PipelineMultisampleStateCreateInfo multisampleInfo{};

	vk::PipelineDepthStencilStateCreateInfo stencilInfo{};//nullptr

	vk::PipelineColorBlendStateCreateInfo blendInfo{};
	vk::PipelineColorBlendAttachmentState attachment{};

	vk::PipelineDynamicStateCreateInfo dynamicState{};
	std::vector<vk::DynamicState> dynamicStates = {
	vk::DynamicState::eViewport,
	vk::DynamicState::eScissor	};

	vk::PipelineLayoutCreateInfo layoutInfo{};
	vk::PushConstantRange  pcRange{};
	std::vector<vk::DescriptorSetLayout> descriptorSetLayouts{};

	vk::PipelineDepthStencilStateCreateInfo depthInfo{};

	vk::GraphicsPipelineCreateInfo createPipelineInfo{};
	
	vk::Pipeline pipeline;
};