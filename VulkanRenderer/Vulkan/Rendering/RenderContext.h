#pragma once
#include <vulkan/vulkan.h>
#include "SwapChain.h"
#include "Pipeline.h"
#include "../Core/VulkanContext.h"
#include "DescriptorPool.h"
#include "DepthPrepass.h"

class RenderContext final
{
public:
	RenderContext(Window& window, VulkanContext& context)
		: m_SwapChain(window, context.GetSurface(), context.GetDevice())
		, m_DepthPrepass(context.GetDevice(), context.GetDevice().FindDepthFormat())
		, m_Pipeline(context.GetDevice(), std::vector({ m_SwapChain.GetSwapChainImageFormat() }), context.GetDevice().FindDepthFormat())
		, m_DescriptorPool(context.GetDevice())
	{
	}

	~RenderContext() = default;

	SwapChain& GetSwapChain()  { return m_SwapChain; }

	DepthPrepass& GetDepthPrepass() { return m_DepthPrepass; };
	Pipeline& GetPipeline() { return m_Pipeline; }
	DescriptorPool& GetDescriptorPool() { return m_DescriptorPool; }

	void InitializeSwapChain(CommandPool& commandPool) { m_SwapChain.Init(commandPool); }

	void SetViewportScissor(VkCommandBuffer commandBuffer)
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

	
	void BeginDynamicRendering(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		m_SwapChain.BeginDynamicRendering(commandBuffer, imageIndex);
	}

	void EndDynamicRendering(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		m_SwapChain.EndDynamicRendering(commandBuffer, imageIndex);	
	}

	void BindPipeline(VkCommandBuffer commandBuffer) { m_Pipeline.Bind(commandBuffer); };
	VkPipelineLayout GetPipelineLayout() const { return m_Pipeline.GetPipelineLayout(); };

	
		//HandleResize()

private:
	SwapChain m_SwapChain;
	Pipeline m_Pipeline;
	DepthPrepass m_DepthPrepass;
	DescriptorPool m_DescriptorPool;
};