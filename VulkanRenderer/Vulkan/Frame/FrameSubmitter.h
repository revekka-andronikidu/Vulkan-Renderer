#pragma once
#include <vulkan/vulkan.h>
#include "../Core/Device.h"

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
        VkCommandBufferSubmitInfo commandBufferInfo{};
        commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        commandBufferInfo.commandBuffer = commandBuffer;

        VkSemaphoreSubmitInfo waitInfo{};
        waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        waitInfo.semaphore = waitSemaphore;
        waitInfo.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSemaphoreSubmitInfo signalInfo{};
        signalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        signalInfo.semaphore = signalSemaphore;
        signalInfo.stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;

        VkSubmitInfo2 submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submitInfo.waitSemaphoreInfoCount = 1;
        submitInfo.pWaitSemaphoreInfos = &waitInfo;
        submitInfo.commandBufferInfoCount = 1;
        submitInfo.pCommandBufferInfos = &commandBufferInfo;
        submitInfo.signalSemaphoreInfoCount = 1;
        submitInfo.pSignalSemaphoreInfos = &signalInfo;

        if (vkQueueSubmit2(m_Device.GetGraphicsQueue(), 1, &submitInfo, fence) != VK_SUCCESS)
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
