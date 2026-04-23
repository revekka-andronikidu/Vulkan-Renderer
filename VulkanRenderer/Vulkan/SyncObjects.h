#pragma once
#include <vulkan/vulkan.h>

class Device;
class SyncObjects
{
public:
    SyncObjects(Device& device)
        : m_Device(device)
    {
		CreateSyncObjects();
    }

    ~SyncObjects()
    {
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_Device.GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device.GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
            vkDestroyFence(m_Device.GetDevice(), m_InFlightFences[i], nullptr);
        }
    }

    void WaitForFence(uint32_t frameIndex)
    {
        vkWaitForFences(m_Device.GetDevice(), 1, &m_InFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
    }

    void ResetFence(uint32_t frameIndex)
    {
        vkResetFences(m_Device.GetDevice(), 1, &m_InFlightFences[frameIndex]);
    }

    VkSemaphore GetImageAvailableSemaphore(uint32_t frameIndex) const { return m_ImageAvailableSemaphores[frameIndex]; }
    VkSemaphore GetRenderFinishedSemaphore(uint32_t frameIndex) const { return m_RenderFinishedSemaphores[frameIndex]; }
    VkFence     GetInFlightFence(uint32_t frameIndex)           const { return m_InFlightFences[frameIndex]; }

private:
    Device& m_Device;
    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence>     m_InFlightFences;

    void CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create sync objects!");
        }
	}
};
