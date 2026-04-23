#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class Device;
class CommandPool
{
public:
	CommandPool(Device& device);
	~CommandPool();

	VkCommandBuffer BeginSingleTimeCommands() const;
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	VkCommandBuffer BeginRecording(uint32_t frameIndex);
	void EndRecording(uint32_t frameIndex);


	VkCommandPool GetCommandPool() const { return m_CommandPool; }
	std::vector<VkCommandBuffer>& GetCommandBuffers() { return m_CommandBuffers; }

private:
	Device& m_Device;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	void CreateCommandPool();
	void CreateCommandBuffers();

};
