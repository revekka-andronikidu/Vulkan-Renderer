#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class Device;
class DescriptorPool final
{
public:
	DescriptorPool(Device& device);
	~DescriptorPool();

	VkDescriptorPool GetHandle() const { return m_DescriptorPool; }
	VkDescriptorSet AllocateSets(VkDescriptorSetLayout layout);

	DescriptorPool(const DescriptorPool&) = delete;
	DescriptorPool& operator=(const DescriptorPool&) = delete;
	DescriptorPool(DescriptorPool&&) = delete;
	DescriptorPool& operator=(DescriptorPool&&) = delete;

private:
	Device& m_Device;
	VkDescriptorPool m_DescriptorPool;
	void CreateDescriptorPool();


};

