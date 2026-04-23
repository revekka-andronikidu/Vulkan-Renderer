#pragma once
#include <vulkan/vulkan.h>
#include "../VulkanContext/Device.h"

class TextureSampler final
{
public:
	TextureSampler(Device& device) : m_Device(device)
    {
        CreateSampler();
	}

    ~TextureSampler()
    {
		vkDestroySampler(m_Device.GetDevice(), m_Sampler, nullptr);
    }

	VkSampler GetSampler() const { return m_Sampler; }

	TextureSampler(const TextureSampler&) = delete;
	TextureSampler& operator=(const TextureSampler&) = delete;
	TextureSampler(TextureSampler&&) = delete;
	TextureSampler& operator=(TextureSampler&&) = delete;

private:
	Device& m_Device;
    VkSampler m_Sampler;

	void CreateSampler()
	{
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_Device.GetPhysicalDevice(), &properties);

        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
	}
};