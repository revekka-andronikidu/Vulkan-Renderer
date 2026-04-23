#pragma once
#include <vulkan/vulkan.h>
#include "Device.h"

class Device;
class FrameSubmitter final
{
public:
    FrameSubmitter(Device& device) : m_Device(device) {}

    // Returns VK_ERROR_OUT_OF_DATE_KHR / VK_SUBOPTIMAL_KHR if resize needed
    VkResult AcquireNextImage(VkSwapchainKHR swapChain, VkSemaphore imageAvailable, uint32_t& imageIndex)
    {
        return vkAcquireNextImageKHR(m_Device.GetDevice(), swapChain,
            UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);
    }

    void Submit(VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore,
        VkSemaphore signalSemaphore, VkFence fence)
    {
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &waitSemaphore;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &signalSemaphore;

        if (vkQueueSubmit(m_Device.GetGraphicsQueue(), 1, &submitInfo, fence) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");    
    }

    VkResult Present(VkSwapchainKHR swapChain, VkSemaphore waitSemaphore, uint32_t imageIndex)
    {
        VkSwapchainKHR swapChains[] = { swapChain };

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &waitSemaphore;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        return vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);
    }

private:
    Device& m_Device;
};
