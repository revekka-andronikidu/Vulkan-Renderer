#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <memory> 

class Image;
class Device;
class Window;
class CommandPool;
class SwapChain final
{
public:
	SwapChain(Window& window, VkSurfaceKHR surface, Device& device, CommandPool& commandPool);
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;


	VkSwapchainKHR GetHandle() const { return m_SwapChain; }
	const std::vector<Image>& GetSwapChainImages() const;
	void RecreateSwapChain(VkRenderPass renderPass);
	VkExtent2D GetExtent() const { return m_SwapChainExtent; }
	std::vector<VkFramebuffer> GetSwapChainFramebuffers() const { return m_SwapChainFramebuffers; }
	VkFormat GetSwapChainImageFormat() const;

	void BeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, uint32_t imageIndex);
	void EndRenderPass(VkCommandBuffer commandBuffer);

	void CreateFramebuffers(VkRenderPass renderPass);


private:
	Device& m_Device;
	Window& m_Window;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_SwapChain;
	std::unique_ptr<Image> m_DepthImage;
	std::vector<Image> m_SwapChainImages;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	CommandPool& m_CommandPool;

	/*std::vector<VkImageView> imageViews;
	VkFormat imageFormat;*/
	VkExtent2D m_SwapChainExtent;

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CleanupSwapChain();
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateDepthResources();
	void CreateImageViews();
};