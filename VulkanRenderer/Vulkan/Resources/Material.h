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
class Material final
{
public:
    Material(Device& device, CommandPool& commandPool, DescriptorPool& pool, VkDescriptorSetLayout descriptorSetLayout, const std::string& texturePath, std::shared_ptr<TextureSampler> textureSampler);
	~Material();


	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;
	Material(Material&&) = delete;
	Material& operator=(Material&&) = delete;

    void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const;

private:
	Device& m_Device;
	CommandPool& m_CommandPool;
	const std::string& m_TexturePath;
    std::unique_ptr<Image> m_TextureImage;
    std::vector<VkDescriptorSet> m_DescriptorSets;
	std::shared_ptr<TextureSampler> m_TextureSampler;

    void CreateTextureImage();
	void WriteDescriptorSet();

};

