
#include "vk_renderer.hpp"
#include "shared_definitions.hpp"

void Renderer::beginRenderPass(vk::CommandBuffer cmdBuffer, vk::ImageView imgView,  vk::Extent2D swapchainExtent) {

	vk::ClearColorValue clr{};	//sinf(counter)
	clr.setFloat32({ 0.1f, 0.1f, 0.2f, 1.0f });

	//setup for dynrenderin
	vk::RenderingAttachmentInfo attachInfo{};
	attachInfo
		.setImageView(imgView)
		.setImageLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setClearValue(clr)
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStoreOp(vk::AttachmentStoreOp::eStore);

	
	vk::RenderingInfoKHR renderInfo{};

	vk::Rect2D area;
	area.setOffset(vk::Offset2D(0, 0)).setExtent(swapchainExtent);
	renderInfo
		.setRenderArea(area)
		.setLayerCount(1)
		.setColorAttachments(attachInfo)
		.setPDepthAttachment(nullptr);
		
	cmdBuffer.beginRendering(renderInfo);

}
void Renderer::recordDraw(vk::CommandBuffer cmdBuffer,vk::Pipeline pipeline, vk::Extent2D swapchainExtent, float extentH, float extentW) {
	
	cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	cmdBuffer.setViewport(0, vk::Viewport(0.0f, extentH, extentW, -extentH, 0.0f, 1.0f));
	cmdBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapchainExtent));
	cmdBuffer.draw(3,1,0,0);


}