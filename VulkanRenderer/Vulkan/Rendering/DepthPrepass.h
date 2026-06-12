#pragma once
#include <vulkan/vulkan.h>
#include "Pipeline.h"

class Device;
class SwapChain;
class CommandPool;

class DepthPrepass final
{
public:
    DepthPrepass(Device& device, VkFormat depthFormat);

    void SetTextureCount(uint32_t count) { m_Pipeline.SetTextureCount(count); }
    void Build() { m_Pipeline.Build(); }

    Pipeline& GetPipeline() { return m_Pipeline; }

    void Begin(VkCommandBuffer commandBuffer, SwapChain& swapChain);
    void End(VkCommandBuffer commandBuffer, SwapChain& swapChain);

private:
    Pipeline m_Pipeline;
};