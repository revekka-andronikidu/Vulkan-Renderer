#include <vulkan/vulkan.h>
#include "Mesh.h"
#include "Material.h"

class CommandPool;
class Device;
class DescriptorPool;
class Model final
{
public:
	Model(Device& device, CommandPool& commandPool, DescriptorPool& descriptorPool, VkDescriptorSetLayout descriptorSetLayout, const std::string& modelPath, const std::string& texturePath, std::shared_ptr<TextureSampler> textureSampler)
		: m_Mesh(device, commandPool, modelPath)
		, m_Material(device, commandPool, descriptorPool, descriptorSetLayout, texturePath, textureSampler)
	{
	}
	~Model() = default;

	void Draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, uint32_t frameIndex) const
	{
		m_Material.Bind(commandBuffer, pipelineLayout, frameIndex);
		m_Mesh.Bind(commandBuffer);
		m_Mesh.Draw(commandBuffer);
	}

private:
	Mesh m_Mesh;
	Material m_Material;
};