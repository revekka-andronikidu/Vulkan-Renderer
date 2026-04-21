#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class Device;
class Pipeline final
{
public:
	Pipeline(Device& device, VkFormat swapChainImageFormat);
	~Pipeline();

	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline(Pipeline&&) = delete;
	Pipeline& operator=(Pipeline&&) = delete;

	VkRenderPass GetRenderPass() const { return m_RenderPass; }
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
	VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }
	VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }


private:
	Device& m_Device;
	VkRenderPass m_RenderPass;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkFormat m_SwapChainImageFormat;

	void CreateGraphicsPipeline();
	void CreateDescriptorSetLayout();
	void CreateRenderPass();
	VkShaderModule CreateShaderModule(const std::vector<char>& code);

};
