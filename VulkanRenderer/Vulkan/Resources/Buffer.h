#pragma once
#include <vulkan/vulkan.h>

class Device;
class CommandPool;

class Buffer final
{
public:
	Buffer(Device& device, CommandPool& commandPool, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	~Buffer();

	void CopyBuffer(VkBuffer srcBuffer, VkDeviceSize size) const;
	VkBuffer GetBuffer() const { return m_Buffer; };
	VkDeviceMemory GetMemory() const {return m_BufferMemory;}

	void Upload(const void* srcData, VkDeviceSize size);

	Buffer(const Buffer& other) = delete;
	Buffer& operator=(const Buffer& other) = delete;
	Buffer(Buffer&& other) = default;
	Buffer& operator=(Buffer&& other) = delete;

protected:
	Device& m_Device;
	VkBuffer m_Buffer;
	VkDeviceMemory m_BufferMemory;
	CommandPool& m_CommandPool;

private:
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

};

