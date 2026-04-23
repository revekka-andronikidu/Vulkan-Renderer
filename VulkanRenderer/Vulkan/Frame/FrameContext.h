#pragma once
#include "CommandPool.h"
#include "FrameSubmitter.h"
#include "SyncObjects.h"
#include "UniformBuffer.h"
#include "../Rendering/RenderContext.h"

class FrameContext final
{
public:
    FrameContext(Device& device, RenderContext& renderContext)
        : m_CommandPool(device)
        , m_SyncObjects(device.GetDevice())
        , m_Submitter(device)
        , m_UniformBuffer(device, m_CommandPool, renderContext)
    {
    }

    CommandPool& GetCommandPool() { return m_CommandPool; };
	UniformBuffer& GetUniformBuffer() { return m_UniformBuffer; };

   void BeginRecording(uint32_t imageIndex){ m_CommandPool.BeginRecording(imageIndex); }
   void EndRecording(uint32_t imageIndex) { m_CommandPool.EndRecording(imageIndex); }
   void BindFrameDescriptors(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) { m_UniformBuffer.Bind(commandBuffer, layout, frameIndex); };
   void WaitForFence(uint32_t frameIndex) { m_SyncObjects.WaitForFence(frameIndex); }
   void ResetFence(uint32_t frameIndex) {m_SyncObjects.ResetFence(frameIndex); }
   VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const { return m_SyncObjects.GetImageAvailableSemaphore(frameIndex); }
   VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const { return m_SyncObjects.GetRenderFinishedSemaphore(frameIndex); }

   void SubmitAndPresent(RenderContext& renderContext, uint32_t frameIndex, uint32_t imageIndex)
   {
	   m_Submitter.Submit(m_CommandPool.GetCommandBuffers()[frameIndex], GetImageAvailableSemaphore(frameIndex), GetRenderFinishedSemaphore(frameIndex), m_SyncObjects.GetInFlightFence(frameIndex) );
	   VkResult result = m_Submitter.Present(renderContext.GetSwapChain().GetHandle(), m_SyncObjects.GetRenderFinishedSemaphore(frameIndex), imageIndex);
       if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_FramebufferResized) {
           m_FramebufferResized = false;
           renderContext.GetSwapChain().RecreateSwapChain(renderContext.GetRenderPass().GetRenderPass(), m_CommandPool);
       }
       else if (result != VK_SUCCESS) {
           throw std::runtime_error("failed to present swap chain image!");
       }
   }
   void AcquireImage(RenderContext& renderContext, uint32_t frameIndex, uint32_t& imageIndex)
   {
       VkResult result = m_Submitter.AcquireNextImage(renderContext.GetSwapChain().GetHandle(), GetImageAvailableSemaphore(frameIndex), imageIndex);

       if (result == VK_ERROR_OUT_OF_DATE_KHR)
       {
           renderContext.GetSwapChain().RecreateSwapChain(renderContext.GetRenderPass().GetRenderPass(), m_CommandPool);
           return;
       }
       else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
           throw std::runtime_error("failed to acquire swap chain image!");
       }
   }



private:
    CommandPool m_CommandPool;
    SyncObjects m_SyncObjects;
    FrameSubmitter m_Submitter;
    UniformBuffer m_UniformBuffer;

	bool m_FramebufferResized = false;
};
