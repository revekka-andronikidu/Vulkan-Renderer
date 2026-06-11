#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "RenderPass.h"

class Device;
class Pipeline final
{
public:
	Pipeline(Device& device, std::vector<VkFormat> colorFormats, VkFormat depthFormat);
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
	//VkRenderPass m_RenderPass;
	VkFormat m_DepthFormat;
	uint32_t m_TextureCount = 0;

	void CreateGraphicsPipeline();
	void CreateMaterialDescriptorSetLayout();
	void CreateGlobalDescriptorSetLayout();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	

};
