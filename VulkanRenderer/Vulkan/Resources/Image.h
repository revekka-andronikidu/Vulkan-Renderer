#pragma once
#include <vulkan/vulkan.h>
#include "TextureSampler.h"

class Device;
class CommandPool;
class Image final
{
public:
	VkImage m_Image;
	VkDeviceMemory m_Memory;
	VkImageView m_ImageView;
	VkFormat m_Format;
	VkImageAspectFlags m_AspectMask;
	uint32_t m_MipLevels;

	Image(Device& device, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits samples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
	Image(Device& device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectMask);
	~Image();

	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&& other) noexcept;
	Image& operator=(Image&& other) noexcept;


	void GenerateMipmaps(CommandPool& commandPool);
	void TransitionImageLayout(CommandPool& commandPool, VkPipelineStageFlags sourceStage, VkPipelineStageFlags destinationStage, VkAccessFlags sourceAccessMask, VkAccessFlags destinationAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CopyBufferToImage(CommandPool& commandPool, VkBuffer buffer);
	void CreateImageView();

private:
	Device& m_Device;
	uint32_t m_TextWidth;
	uint16_t m_TextHeight;
	void CreateImage(uint32_t width, uint32_t height, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage);
	void Cleanup();
	bool m_OwnsImage;
};
