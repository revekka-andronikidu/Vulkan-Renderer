#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include "Resources/Buffer.h"

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

class Device;
class CommandPool;
class DescriptorPool;
class UniformBuffer final
{
public:
    UniformBuffer(Device& device, CommandPool& commandPool, DescriptorPool& pool, VkDescriptorSetLayout globalLayout, uint32_t maxFramesInFlight);
    ~UniformBuffer() = default;

    UniformBuffer(const UniformBuffer&) = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer(UniformBuffer&&) = delete;
    UniformBuffer& operator=(UniformBuffer&&) = delete;

    void Update(uint32_t frameIndex, const UniformBufferObject& uniformBuffer);
    void Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const;

private:
    Device& m_Device;
    CommandPool& m_CommandPool;
    uint32_t m_MaxFramesInFlight;
    std::vector<void*> m_UniformBuffersMapped;
    std::vector<Buffer> m_UniformBuffers;
    std::vector<VkDescriptorSet> m_DescriptorSets;

    void CreateUniformBuffers();
    void WriteDescriptorSets();

};