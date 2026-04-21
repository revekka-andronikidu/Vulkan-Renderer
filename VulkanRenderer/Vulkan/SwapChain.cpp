#include "SwapChain.h"
#include "Device.h"
#include "ResourcesUtils.h"
#include "../Window.h"

#include <stdexcept>
#include <algorithm>
#include <GLFW/glfw3.h>


SwapChain::SwapChain(Window& window, VkSurfaceKHR surface, Device& device)
	: m_Window(window)
    , m_Device(device)
    , m_Surface(surface)
	, m_SwapChain(VK_NULL_HANDLE)
	, m_SwapChainExtent({ 0, 0 })
    , m_SwapChainImages{}
{ 
	CreateSwapChain();
    CreateImageViews();
}

SwapChain::~SwapChain()
{
    CleanupSwapChain();
}
void SwapChain::CreateSwapChain()
{
    Device::SwapChainSupportDetails swapChainSupport = m_Device.querySwapChainSupport(m_Device.GetPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_Surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    Device::QueueFamilyIndices indices = m_Device.FindQueueFamilies(m_Device.GetPhysicalDevice());
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(m_Device.GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_SwapChain, &imageCount, nullptr);
    std::vector<VkImage> rawImages(imageCount);
    vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_SwapChain, &imageCount, rawImages.data());

    m_SwapChainImages.resize(imageCount);
    for (uint32_t i = 0; i < imageCount; i++)
    {
        m_SwapChainImages[i].image = rawImages[i];
        m_SwapChainImages[i].memory = VK_NULL_HANDLE;
        m_SwapChainImages[i].view = VK_NULL_HANDLE;
        m_SwapChainImages[i].format = surfaceFormat.format;;
        m_SwapChainImages[i].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        m_SwapChainImages[i].mipLevels = 1;
    }

    m_SwapChainExtent = extent;
}

VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

void SwapChain::CreateImageViews()
{
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        CreateImageView(m_Device, m_SwapChainImages[i]);
    }
}

void SwapChain::RecreateSwapChain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_Device.GetDevice());
    CleanupSwapChain();

    CreateSwapChain();
    CreateImageViews();
    //createDepthResources();
    //createFramebuffers();
}

void SwapChain::CleanupSwapChain() {

    /*vkDestroyImageView(device.GetDevice(), depthImage.view, nullptr);
    vkDestroyImage(device.GetDevice(), depthImage.image, nullptr);
    vkFreeMemory(device.GetDevice(), depthImage.memory, nullptr);

    for (auto framebuffer : swapChainFramebuffers) {
        vkDestroyFramebuffer(device.GetDevice(), framebuffer, nullptr);
    }*/

    for (auto imageView : m_SwapChainImages) {
        vkDestroyImageView(m_Device.GetDevice(), imageView.view, nullptr);
    }

    vkDestroySwapchainKHR(m_Device.GetDevice(), m_SwapChain, nullptr);
}

VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(m_Window.GetWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

const std::vector<Image>& SwapChain::GetSwapChainImages() const
{
    return m_SwapChainImages;
}

VkFormat SwapChain::GetSwapChainImageFormat() const
{
    return m_SwapChainImages.empty() ? VK_FORMAT_UNDEFINED : m_SwapChainImages[0].format;
}