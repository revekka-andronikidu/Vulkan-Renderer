#include "../Window.h"
#include "Core/VulkanContext.h"
#include "Rendering/RenderContext.h"
#include "Frame/FrameContext.h"
#include "Resources/ResourceManager.h"
#include "Camera.h"

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
    VulkanContext m_VulkanContext;
    RenderContext m_RenderContext;
    FrameContext m_FrameContext;
    ResourceManager m_ResourceManager;

    Camera m_Camera;

    uint32_t m_CurrentFrame = 0;
   
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void DrawFrame();   
};
