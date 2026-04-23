#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class Device;
class DescriptorPool final
{
public:
	DescriptorPool(Device& device, uint32_t maxSets);
	~DescriptorPool();

	VkDescriptorPool GetHandle() const { return m_DescriptorPool; }
	std::vector<VkDescriptorSet> AllocateSets(VkDescriptorSetLayout layout);

	DescriptorPool(const DescriptorPool&) = delete;
	DescriptorPool& operator=(const DescriptorPool&) = delete;
	DescriptorPool(DescriptorPool&&) = delete;
	DescriptorPool& operator=(DescriptorPool&&) = delete;

private:
	Device& m_Device;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet> m_DescriptorSets;
	uint32_t m_MaxFramesInFlight;
	void CreateDescriptorPool();


};

