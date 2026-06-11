#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <memory>
#include <vector>

class Device;
class CommandPool;
class Image;
class DescriptorPool;
class TextureSampler;
class TextureArray final
{
public:
	TextureArray(Device& device, CommandPool& commandPool, DescriptorPool& pool, VkDescriptorSetLayout descriptorSetLayout);
	~TextureArray();

	TextureArray(const TextureArray&) = delete;
	TextureArray& operator=(const TextureArray&) = delete;
	TextureArray(TextureArray&&) = delete;
	TextureArray& operator=(TextureArray&&) = delete;

	uint32_t AddTexture(const std::string& path);
    void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const;
	void Build();
	uint32_t GetTextureCount() const { return static_cast<uint32_t>(m_TextureImages.size()); }

private:
	Device& m_Device;
	CommandPool& m_CommandPool;
	DescriptorPool& m_Pool;
	VkDescriptorSetLayout m_Layout;
	std::vector<std::unique_ptr<Image>> m_TextureImages;
    std::vector<VkDescriptorSet> m_DescriptorSets;
	std::shared_ptr<TextureSampler> m_TextureSampler;

    void CreateTextureImage(const std::string& path);
	void WriteDescriptorSets();
};
