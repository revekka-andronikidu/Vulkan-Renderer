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
	SwapChain(Window& window, VkSurfaceKHR surface, Device& device);
	~SwapChain();

	SwapChain(const SwapChain&) = delete;
	SwapChain& operator=(const SwapChain&) = delete;
	SwapChain(SwapChain&&) = delete;
	SwapChain& operator=(SwapChain&&) = delete;


	VkSwapchainKHR GetHandle() const { return m_SwapChain; }
	const std::vector<Image>& GetSwapChainImages() const;
	void RecreateSwapChain( CommandPool& commandPool);
	VkExtent2D GetExtent() const { return m_SwapChainExtent; }
	Image* GetDepthImage() const { return m_DepthImage.get(); };
	void BeginDynamicRendering(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void EndDynamicRendering(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	
	VkFormat GetSwapChainImageFormat() const;

	void Init(CommandPool& commandPool)
	{
		CreateDepthResources(commandPool);
	}

private:
	Device& m_Device;
	Window& m_Window;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_SwapChain;
	std::unique_ptr<Image> m_DepthImage;
	std::vector<Image> m_SwapChainImages;
	VkExtent2D m_SwapChainExtent;

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CleanupSwapChain();
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateDepthResources(CommandPool& commandPool);
	void CreateImageViews();

};