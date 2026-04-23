#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
	: m_Window()
	, m_Context(m_Window)
	, m_CommandPool(m_Context.GetDevice())
	, m_SwapChain(m_Window, m_Context.GetSurface(), m_Context.GetDevice(), m_CommandPool)
	, m_RenderPass(m_Context.GetDevice().GetDevice(), m_SwapChain.GetSwapChainImageFormat(), m_Context.GetDevice().FindDepthFormat())
	, m_Pipeline(m_Context.GetDevice(), m_SwapChain.GetSwapChainImageFormat(), m_RenderPass.GetRenderPass())
	, m_DescriptorPool(m_Context.GetDevice(), static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT))
	, m_UniformBuffer(m_Context.GetDevice(), m_CommandPool, m_DescriptorPool, m_Pipeline.GetGlobalSetLayout(),  static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT))
	, m_SyncObjects(m_Context.GetDevice())
	, m_Submitter(m_Context.GetDevice())
	, m_ResourceManager(m_Context.GetDevice(), m_CommandPool, m_DescriptorPool, m_Pipeline.GetMaterialSetLayout())

{
	m_SwapChain.CreateFramebuffers(m_RenderPass.GetRenderPass());
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Run()
{
	while (!m_Window.ShouldClose())
	{
		glfwPollEvents();
		DrawFrame();
	}
	vkDeviceWaitIdle(m_Context.GetDevice().GetDevice());
}

void VulkanRenderer::DrawFrame()
{
	m_SyncObjects.WaitForFence(m_CurrentFrame);

	uint32_t imageIndex;
	VkResult result = m_Submitter.AcquireNextImage(m_SwapChain.GetHandle(), m_SyncObjects.GetImageAvailableSemaphore(m_CurrentFrame), imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		m_SwapChain.RecreateSwapChain(m_RenderPass.GetRenderPass());
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	m_Camera.UpdateUniformBuffer(m_CurrentFrame, m_UniformBuffer, m_SwapChain.GetExtent());

	m_SyncObjects.ResetFence(m_CurrentFrame);

	RecordCommandBuffer(m_CommandPool.GetCommandBuffers()[m_CurrentFrame], imageIndex);	
	m_Submitter.Submit(m_CommandPool.GetCommandBuffers()[m_CurrentFrame], m_SyncObjects.GetImageAvailableSemaphore(m_CurrentFrame), m_SyncObjects.GetRenderFinishedSemaphore(m_CurrentFrame), m_SyncObjects.GetInFlightFence(m_CurrentFrame));

	result = m_Submitter.Present(m_SwapChain.GetHandle(), m_SyncObjects.GetRenderFinishedSemaphore(m_CurrentFrame), imageIndex);


	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
		m_FramebufferResized = false;
		m_SwapChain.RecreateSwapChain(m_RenderPass.GetRenderPass());
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	m_CommandPool.BeginRecording(imageIndex);
	m_SwapChain.BeginRenderPass(commandBuffer, m_RenderPass.GetRenderPass(), imageIndex);
	m_Pipeline.Bind(commandBuffer);

	SetViewportScissor(commandBuffer);

	m_UniformBuffer.Bind(commandBuffer, m_Pipeline.GetPipelineLayout(), m_CurrentFrame);
	m_ResourceManager.DrawAll(commandBuffer, m_Pipeline.GetPipelineLayout(), m_CurrentFrame);
	m_SwapChain.EndRenderPass(commandBuffer);
	m_CommandPool.EndRecording(imageIndex);
}

void VulkanRenderer::SetViewportScissor(VkCommandBuffer commandBuffer)
{
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChain.GetExtent().width);
	viewport.height = static_cast<float>(m_SwapChain.GetExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_SwapChain.GetExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}


