#include "UniformBuffer.h"
#include "VulkanContext/Device.h"
#include "CommandPool.h"
#include "Resources/DescriptorPool.h"


UniformBuffer::UniformBuffer(Device& device, CommandPool& commandPool, DescriptorPool& pool,VkDescriptorSetLayout globalLayout, uint32_t maxFramesInFlight)
    : m_Device(device)
    , m_CommandPool(commandPool)
	, m_MaxFramesInFlight(maxFramesInFlight)
    , m_UniformBuffers()
	, m_UniformBuffersMapped()
	, m_DescriptorSets()
{
    CreateUniformBuffers();
    m_DescriptorSets = pool.AllocateSets(globalLayout);
    WriteDescriptorSets();
}


void UniformBuffer::CreateUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_UniformBuffers.reserve(m_MaxFramesInFlight);
    m_UniformBuffersMapped.resize(m_MaxFramesInFlight);

    for (size_t i = 0; i < m_MaxFramesInFlight; i++) {
        m_UniformBuffers.emplace_back(m_Device, m_CommandPool, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        vkMapMemory(m_Device.GetDevice(), m_UniformBuffers[i].GetMemory(), 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
    }
}

void UniformBuffer::WriteDescriptorSets()
{
    for (size_t i = 0; i < m_MaxFramesInFlight; i++)
    {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_UniformBuffers[i].GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(m_Device.GetDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

void UniformBuffer::Update(uint32_t frameIndex, const UniformBufferObject& ubo)
{
    memcpy(m_UniformBuffersMapped[frameIndex], &ubo, sizeof(ubo));
}

void UniformBuffer::Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_DescriptorSets[frameIndex], 0, nullptr);
}