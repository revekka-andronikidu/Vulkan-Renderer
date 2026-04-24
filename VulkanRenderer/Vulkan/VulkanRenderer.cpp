#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
	: m_Window()
	, m_VulkanContext(m_Window)
	, m_RenderContext(m_Window, m_VulkanContext)
	, m_FrameContext(m_VulkanContext.GetDevice(), m_RenderContext)
	, m_ResourceManager(m_VulkanContext.GetDevice(), m_RenderContext, m_FrameContext)

{
	m_RenderContext.InitializeSwapChain(m_FrameContext.GetCommandPool());
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
	vkDeviceWaitIdle(m_VulkanContext.GetDevice().GetDevice());
}

void VulkanRenderer::DrawFrame()
{
	m_FrameContext.WaitForFence(m_CurrentFrame);

	uint32_t imageIndex;
	m_FrameContext.AcquireImage(m_RenderContext, m_CurrentFrame, imageIndex);

	m_Camera.UpdateUniformBuffer(m_CurrentFrame, m_FrameContext.GetUniformBuffer(), m_RenderContext.GetSwapChain().GetExtent());

	m_FrameContext.ResetFence(m_CurrentFrame);

	RecordCommandBuffer(m_FrameContext.GetCommandPool().GetCommandBuffers()[m_CurrentFrame], imageIndex);

	m_FrameContext.SubmitAndPresent(m_RenderContext, m_CurrentFrame, imageIndex);


	m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	m_FrameContext.BeginRecording(imageIndex);

	m_RenderContext.BeginRenderPass(commandBuffer, imageIndex);
	m_RenderContext.BindPipeline(commandBuffer);
	m_RenderContext.SetViewportScissor(commandBuffer);

	m_FrameContext.BindFrameDescriptors(commandBuffer, m_RenderContext.GetPipelineLayout(), m_CurrentFrame);
	m_ResourceManager.GetTextureArray().Bind(commandBuffer, m_RenderContext.GetPipelineLayout(), m_CurrentFrame);

	m_ResourceManager.DrawAll(commandBuffer, m_RenderContext.GetPipeline(), m_CurrentFrame);

	m_RenderContext.EndRenderPass(commandBuffer);
	m_FrameContext.EndRecording(imageIndex);
}




