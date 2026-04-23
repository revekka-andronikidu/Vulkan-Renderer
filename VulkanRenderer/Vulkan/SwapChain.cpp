#include "SwapChain.h"
#include "Device.h"
#include "ResourcesUtils.h"
#include "../Window.h"

#include <stdexcept>
#include <algorithm>
#include <GLFW/glfw3.h>
#include "Resources/Image.h"
#include "CommandPool.h"

SwapChain::SwapChain(Window& window, VkSurfaceKHR surface, Device& device, CommandPool& commandPool)
	: m_Window(window)
    , m_Device(device)
    , m_Surface(surface)
	, m_SwapChain(VK_NULL_HANDLE)
	, m_SwapChainExtent({ 0, 0 })
    , m_SwapChainImages{}
	, m_CommandPool(commandPool)
{ 
	CreateSwapChain();
    CreateImageViews(); 

    CreateDepthResources();
   // CreateFramebuffers();
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

    m_SwapChainImages.clear();
    m_SwapChainImages.reserve(imageCount);

    for (uint32_t i = 0; i < imageCount; i++)
    {
        m_SwapChainImages.emplace_back(
            m_Device,
            rawImages[i],
            surfaceFormat.format,
            VK_IMAGE_ASPECT_COLOR_BIT
        );
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

void SwapChain::RecreateSwapChain(VkRenderPass renderPass) 
{
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


    CreateDepthResources();
    CreateFramebuffers(renderPass);
}

void SwapChain::CreateImageViews()
{
    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        m_SwapChainImages[i].CreateImageView();
    }
}

void SwapChain::CleanupSwapChain() {

    for (auto framebuffer : m_SwapChainFramebuffers) {
        vkDestroyFramebuffer(m_Device.GetDevice(), framebuffer, nullptr);
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
    return m_SwapChainImages.empty() ? VK_FORMAT_UNDEFINED : m_SwapChainImages[0].m_Format;
}

void SwapChain::CreateDepthResources()
{
    VkFormat depthFormat = m_Device.FindDepthFormat();

    m_DepthImage = std::make_unique<Image>(m_Device, m_SwapChainExtent.width, m_SwapChainExtent.height, 1, VK_SAMPLE_COUNT_1_BIT, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

    m_DepthImage->TransitionImageLayout(m_CommandPool, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void SwapChain::CreateFramebuffers(VkRenderPass renderPass)
{
    m_SwapChainFramebuffers.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        std::array<VkImageView, 2> attachments = 
        {
            m_SwapChainImages[i].m_ImageView,
            m_DepthImage->m_ImageView
        };


        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());;
        framebufferInfo.pAttachments = attachments.data();;
        framebufferInfo.width = m_SwapChainExtent.width;
        framebufferInfo.height = m_SwapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device.GetDevice(), &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void SwapChain::BeginRenderPass(VkCommandBuffer commandBuffer, VkRenderPass renderPass, uint32_t imageIndex)
{
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = m_SwapChainExtent;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());;
    renderPassInfo.pClearValues = clearValues.data();;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void SwapChain::EndRenderPass(VkCommandBuffer cmd)
{
    vkCmdEndRenderPass(cmd);
}