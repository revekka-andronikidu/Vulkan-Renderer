#include <vulkan/vulkan.h>
#include "Mesh.h"
#include "Material.h"
#include "../../Rendering/Pipeline.h"
class CommandPool;
class Device;
class DescriptorPool;
class Model final
{
public:
	Model(Device& device, CommandPool& commandPool, const std::string& modelPath, TextureArray& textureArray,  const std::string& texturePath)
		: m_Mesh(device, commandPool, modelPath)
		, m_Material(textureArray,texturePath)
	{
	}

	Model(Device& device, CommandPool& commandPool,const std::vector<Vertex>& vertices,const std::vector<uint32_t>& indices, TextureArray& textureArray, const std::string& texturePath)
		: m_Mesh(device, commandPool, std::move(vertices), std::move(indices))
		, m_Material(textureArray, texturePath)
	{
	}

	~Model() = default;

	void Draw(VkCommandBuffer commandBuffer, const Pipeline& pipeline, uint32_t frameIndex) const
	{
		pipeline.PushTextureIndex(commandBuffer, m_Material.GetTextureIndex());

		m_Mesh.Bind(commandBuffer);
		m_Mesh.Draw(commandBuffer);
	}

private:
	Mesh m_Mesh;
	Material m_Material;
};