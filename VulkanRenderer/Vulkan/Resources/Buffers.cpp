#include "Buffers.h"
#include <stdexcept>
#include "../Device.h"
#include "../ResourcesUtils.h"
#include "../CommandPool.h"

Buffer::Buffer(Device& device, CommandPool& commandPool, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : m_Device(device.GetDevice())
    , m_PhysicalDevice(device.GetPhysicalDevice())
    , m_CommandPool(commandPool)

{
    CreateBuffer(size, usage, properties);
}

Buffer::~Buffer()
{
    vkDestroyBuffer(m_Device, m_Buffer, nullptr);
    vkFreeMemory(m_Device, m_BufferMemory, nullptr);
}

void Buffer::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) 
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(m_PhysicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }

    vkBindBufferMemory(m_Device, m_Buffer, m_BufferMemory, 0);
}

void Buffer::CopyBuffer(VkBuffer srcBuffer, VkDeviceSize size) const
{
    VkCommandBuffer commandBuffer = m_CommandPool.BeginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, m_Buffer, 1, &copyRegion);

    m_CommandPool.EndSingleTimeCommands(commandBuffer);
}

void Buffer::Upload(const void* srcData, VkDeviceSize size)
{
    void* data = nullptr;

    vkMapMemory(m_Device, m_BufferMemory, 0, size, 0, &data);
    memcpy(data, srcData, (size_t)size);
    vkUnmapMemory(m_Device, m_BufferMemory);
}