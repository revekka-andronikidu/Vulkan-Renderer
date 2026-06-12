#pragma once
#include <vulkan/vulkan.h>
#include <vector>


class Device;
class Pipeline final
{
public:
	Pipeline(Device& device, std::vector<VkFormat> colorFormats, VkFormat depthFormat, bool depthWrite = true, VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
	VkDescriptorSetLayout GetMaterialSetLayout() const { return m_MaterialDescriptorSetLayout; }
	VkDescriptorSetLayout GetGlobalSetLayout() const { return m_GlobalDescriptorSetLayout; }
	void Bind(VkCommandBuffer commandBuffer) const;
	void SetTextureCount(uint32_t count);
	void PushTextureIndex(VkCommandBuffer cmd, uint32_t textureIndex) const;
	void Build();

private:
	Device& m_Device;
	VkPipeline m_GraphicsPipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorSetLayout m_MaterialDescriptorSetLayout;
	VkDescriptorSetLayout m_GlobalDescriptorSetLayout;
	std::vector<VkFormat> m_ColorFormats;
	VkFormat m_DepthFormat;
	uint32_t m_TextureCount = 0;
	bool m_DepthWrite = true;
	VkCompareOp m_DepthCompareOp = VK_COMPARE_OP_LESS;

	void CreateGraphicsPipeline();
	void CreateMaterialDescriptorSetLayout();
	void CreateGlobalDescriptorSetLayout();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	

};
