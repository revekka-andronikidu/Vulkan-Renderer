#include "VulkanRenderer.h"


VulkanRenderer::VulkanRenderer()
	: window()
	, context(window)
	, device(context)
	, commandPool(device)
	, swapChain(window, context.GetSurface(), device, commandPool)
	, m_RenderPass(device.GetDevice(), swapChain.GetSwapChainImageFormat(), device.FindDepthFormat())
	, pipeline(device, swapChain.GetSwapChainImageFormat(), m_RenderPass.GetRenderPass())
	, descriptorPool(device, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT))
	, uniformBuffer(device, commandPool, descriptorPool, pipeline.GetGlobalSetLayout(),  static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT))
	, resourceManager(device, commandPool, descriptorPool, pipeline.GetMaterialSetLayout())
{
	swapChain.CreateFramebuffers(m_RenderPass.GetRenderPass());

	initVulkan();
}

VulkanRenderer::~VulkanRenderer()
{
	cleanup();

}

void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	commandPool.BeginRecording(imageIndex);
	swapChain.BeginRenderPass(commandBuffer, m_RenderPass.GetRenderPass(), imageIndex);
	pipeline.Bind(commandBuffer);

	SetViewportScissor(commandBuffer);

	uniformBuffer.Bind(commandBuffer, pipeline.GetPipelineLayout(), currentFrame);
	resourceManager.DrawAll(commandBuffer, pipeline.GetPipelineLayout(), currentFrame);
	swapChain.EndRenderPass(commandBuffer);
	commandPool.EndRecording(imageIndex);
}

void VulkanRenderer::SetViewportScissor(VkCommandBuffer commandBuffer)
{
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChain.GetExtent().width);
	viewport.height = static_cast<float>(swapChain.GetExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChain.GetExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}


