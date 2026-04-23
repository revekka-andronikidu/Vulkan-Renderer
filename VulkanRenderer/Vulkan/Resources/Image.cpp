#include "Image.h"
#include <stdexcept>
#include "../Core/Device.h"

#include "../Frame/CommandPool.h"

Image::Image(Device& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspectMask)
	: m_Device(device)
	, m_Image(VK_NULL_HANDLE)
	, m_Memory(VK_NULL_HANDLE)
	, m_ImageView(VK_NULL_HANDLE)
	, m_Format(format)
	, m_AspectMask(aspectMask)
	, m_MipLevels(mipLevels)
	, m_TextWidth(width)
	, m_TextHeight(height)
	, m_OwnsImage(true)
{
	CreateImage(width, height, samples, tiling, usage);
	CreateImageView();
}

Image::Image(Device& device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectMask)
    : m_Device(device)
    , m_Image(existingImage)
    , m_Memory(VK_NULL_HANDLE)
    , m_ImageView(VK_NULL_HANDLE)
    , m_Format(format)
    , m_AspectMask(aspectMask)
    , m_MipLevels(1)
    , m_OwnsImage(false)
{
   CreateImageView();
}

Image::~Image()
{
    Cleanup();
}
void Image::Cleanup()
{
    
        vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);

    if (m_OwnsImage && m_Image != VK_NULL_HANDLE)
        vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
    if (m_OwnsImage && m_Memory != VK_NULL_HANDLE)
        vkFreeMemory(m_Device.GetDevice(), m_Memory, nullptr);
}

Image::Image(Image&& other) noexcept
    : m_Device(other.m_Device)
    , m_Image(other.m_Image)
    , m_Memory(other.m_Memory)
    , m_ImageView(other.m_ImageView)
    , m_Format(other.m_Format)
    , m_AspectMask(other.m_AspectMask)
    , m_MipLevels(other.m_MipLevels)
	, m_TextWidth(other.m_TextWidth)
    , m_TextHeight(other.m_TextHeight)
	, m_OwnsImage(other.m_OwnsImage)
{
    other.m_Image = VK_NULL_HANDLE;
    other.m_Memory = VK_NULL_HANDLE;
    other.m_ImageView = VK_NULL_HANDLE;
}

Image& Image::operator=(Image&& other) noexcept
{
    if (this != &other)
    {
        Cleanup();

        m_Image = other.m_Image;
        m_Memory = other.m_Memory;
        m_ImageView = other.m_ImageView;
        m_OwnsImage = other.m_OwnsImage;

        other.m_Image = VK_NULL_HANDLE;
        other.m_Memory = VK_NULL_HANDLE;
        other.m_ImageView = VK_NULL_HANDLE;
        other.m_OwnsImage = false;
    }
    return *this;
}


void Image::CreateImage(uint32_t width, uint32_t height, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = m_MipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = m_Format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = samples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(m_Device.GetDevice(), &imageInfo, nullptr, &m_Image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(m_Device.GetDevice(), m_Image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = m_Device.FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(m_Device.GetDevice(), &allocInfo, nullptr, &m_Memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(m_Device.GetDevice(), m_Image, m_Memory, 0);
}

void Image::CreateImageView()
{
    if(m_ImageView)
		vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = m_Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = m_Format;
    viewInfo.subresourceRange.aspectMask = m_AspectMask;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = m_MipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }
}

void Image::GenerateMipmaps(CommandPool& commandPool) 
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(m_Device.GetPhysicalDevice(), m_Format, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    VkCommandBuffer commandBuffer = commandPool.BeginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = m_Image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = m_TextWidth;
    int32_t mipHeight = m_TextHeight;

    for (uint32_t i = 1; i < m_MipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;

    }

    barrier.subresourceRange.baseMipLevel = m_MipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    commandPool.EndSingleTimeCommands(commandBuffer);
}

void Image::TransitionImageLayout(CommandPool& commandPool, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags sourceAccessMask, VkAccessFlags destinationAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = commandPool.BeginSingleTimeCommands();

    // Perform layout transition using image memory barrier
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_Image;
    barrier.subresourceRange.aspectMask = m_AspectMask;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_MipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = sourceAccessMask;
    barrier.dstAccessMask = destinationAccessMask;

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage,
        destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    commandPool.EndSingleTimeCommands(commandBuffer);
}

void Image::CopyBufferToImage(CommandPool& commandPool, VkBuffer buffer) 
{
    VkCommandBuffer commandBuffer = commandPool.BeginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = m_AspectMask;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        m_TextWidth,
        m_TextHeight,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        m_Image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    commandPool.EndSingleTimeCommands(commandBuffer);
}