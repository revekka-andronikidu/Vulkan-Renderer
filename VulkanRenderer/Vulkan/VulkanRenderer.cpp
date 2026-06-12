#include "VulkanRenderer.h"

VulkanRenderer::VulkanRenderer()
	: m_Window()
	, m_VulkanContext(m_Window)
	, m_RenderContext(m_Window, m_VulkanContext)
	, m_FrameContext(m_VulkanContext.GetDevice(), m_RenderContext)
	, m_ResourceManager(m_VulkanContext.GetDevice(), m_RenderContext, m_FrameContext)

{
	m_RenderContext.InitializeSwapChain(m_FrameContext.GetCommandPool());

	glfwSetWindowUserPointer(m_Window.GetWindow(), this);
	glfwSetCursorPosCallback(m_Window.GetWindow(),
		[](GLFWwindow* w, double x, double y) {
			auto* renderer = static_cast<VulkanRenderer*>(glfwGetWindowUserPointer(w));
			renderer->m_Camera.ProcessMouseMovement(
				static_cast<float>(x), static_cast<float>(y));
		});

	glfwSetScrollCallback(m_Window.GetWindow(),
		[](GLFWwindow* w, double, double y) {
			auto* renderer = static_cast<VulkanRenderer*>(glfwGetWindowUserPointer(w));
			renderer->m_Camera.ProcessMouseScroll(static_cast<float>(y));
		});
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Run()
{
	while (!m_Window.ShouldClose())
	{
		float currentTime = static_cast<float>(glfwGetTime());
		float deltaTime = currentTime - m_LastFrameTime;
		m_LastFrameTime = currentTime;

		glfwPollEvents();
		m_Camera.ProcessInput(m_Window.GetWindow(), deltaTime);

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
	m_FrameContext.BeginRecording(m_CurrentFrame);

	m_RenderContext.BeginDynamicRendering(commandBuffer, imageIndex);

	m_RenderContext.BindPipeline(commandBuffer);
	m_RenderContext.SetViewportScissor(commandBuffer);

	m_FrameContext.BindFrameDescriptors(commandBuffer, m_RenderContext.GetPipelineLayout(), m_CurrentFrame);
	m_ResourceManager.GetTextureArray().Bind(commandBuffer, m_RenderContext.GetPipelineLayout(), m_CurrentFrame);

	m_ResourceManager.DrawAll(commandBuffer, m_RenderContext.GetPipeline(), m_CurrentFrame);

	m_RenderContext.EndDynamicRendering(commandBuffer, imageIndex);

	m_FrameContext.EndRecording(m_CurrentFrame);
}




