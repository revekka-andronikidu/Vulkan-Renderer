#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "RenderPass.h"

class Device;
class Pipeline final
{
public:
	Pipeline(Device& device, VkFormat swapChainImageFormat, VkRenderPass renderPass);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
	VkDescriptorSetLayout GetMaterialSetLayout() const { return m_MaterialDescriptorSetLayout; }
	VkDescriptorSetLayout GetGlobalSetLayout() const { return m_GlobalDescriptorSetLayout; }
	void Bind(VkCommandBuffer commandBuffer) const;

private:
	Device& m_Device;
	VkPipeline m_GraphicsPipeline;
	VkPipelineLayout m_PipelineLayout;
	VkDescriptorSetLayout m_MaterialDescriptorSetLayout;
	VkDescriptorSetLayout m_GlobalDescriptorSetLayout;
	VkFormat m_SwapChainImageFormat;
	VkRenderPass m_RenderPass;

	void CreateGraphicsPipeline();
	void CreateMaterialDescriptorSetLayout();
	void CreateGlobalDescriptorSetLayout();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

};
