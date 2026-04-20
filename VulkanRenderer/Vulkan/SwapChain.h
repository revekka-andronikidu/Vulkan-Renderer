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
	/* 
	const std::vector<VkImageView>& GetImageViews() const { return imageViews; }
	VkFormat GetImageFormat() const { return imageFormat; }*/
	VkExtent2D GetExtent() const { return swapChainExtent; }

private:
	Device& device;
	Window& window;
	VkSurfaceKHR surface;
	VkSwapchainKHR m_SwapChain;
	std::vector<Image> swapChainImages;
	/*std::vector<VkImageView> imageViews;
	VkFormat imageFormat;*/
	VkExtent2D swapChainExtent;

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CleanupSwapChain();
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	void CreateImageViews();
};