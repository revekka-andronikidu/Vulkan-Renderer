#include "DescriptorPool.h"
#include "../VulkanContext/Device.h"
#include <array>

DescriptorPool::DescriptorPool(Device& device, uint32_t maxFramesInFlight)
	: m_Device(device)
	, m_MaxFramesInFlight(maxFramesInFlight)
{
	CreateDescriptorPool();
}

DescriptorPool::~DescriptorPool()
{
	vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
}

void DescriptorPool::CreateDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_MaxFramesInFlight * 2);

	if (vkCreateDescriptorPool(m_Device.GetDevice(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

std::vector<VkDescriptorSet> DescriptorPool::AllocateSets(VkDescriptorSetLayout layout)
{
    std::vector<VkDescriptorSetLayout> layouts(m_MaxFramesInFlight, layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(m_MaxFramesInFlight);
    allocInfo.pSetLayouts = layouts.data();

    m_DescriptorSets.resize(m_MaxFramesInFlight);
    if (vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

	return m_DescriptorSets;
}



