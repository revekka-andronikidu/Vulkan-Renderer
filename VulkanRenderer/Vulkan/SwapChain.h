#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct Image;
class Device;
class Window;

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
	void RecreateSwapChain();
	VkExtent2D GetExtent() const { return m_SwapChainExtent; }
	VkFormat GetSwapChainImageFormat() const;

private:
	Device& m_Device;
	Window& m_Window;
	VkSurfaceKHR m_Surface;
	VkSwapchainKHR m_SwapChain;
	std::vector<Image> m_SwapChainImages;
	/*std::vector<VkImageView> imageViews;
	VkFormat imageFormat;*/
	VkExtent2D m_SwapChainExtent;

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CleanupSwapChain();
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateImageViews();
};