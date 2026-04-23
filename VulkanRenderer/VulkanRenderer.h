#include "Window.h"
#include "Vulkan/Device.h"
#include "Vulkan/Swapchain.h"
#include "Vulkan/ResourcesUtils.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Resources/Image.h"
#include "Vulkan/CommandPool.h"
#include "Vulkan/Resources/Buffer.h"
#include "Vulkan/Resources/ResourceManager.h"
#include "Vulkan/UniformBuffer.h"
#include "Vulkan/SyncObjects.h"
#include "Vulkan/FrameSubmitter.h"
#include "Vulkan/Camera.h"

class VulkanRenderer final
{
public:
    VulkanRenderer();
	VulkanRenderer(const VulkanRenderer&) = delete;
	VulkanRenderer& operator=(const VulkanRenderer&) = delete;
	VulkanRenderer(VulkanRenderer&&) = delete;
	VulkanRenderer& operator=(VulkanRenderer&&) = delete;

	~VulkanRenderer();

    void Run();

private:
    Window m_Window;
    VulkanContext m_Context;
	Device m_Device; //move to context?
    CommandPool m_CommandPool;
	SwapChain m_SwapChain;
	DescriptorPool m_DescriptorPool;
    RenderPass m_RenderPass;
	Pipeline m_Pipeline;
	UniformBuffer m_UniformBuffer;
    ResourceManager m_ResourceManager;
    SyncObjects m_SyncObjects;
    FrameSubmitter m_Submitter;
    Camera m_Camera;

    bool m_FramebufferResized = false;
    uint32_t m_CurrentFrame = 0;
   
    void SetViewportScissor(VkCommandBuffer commandBuffer);
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void DrawFrame();   
};
