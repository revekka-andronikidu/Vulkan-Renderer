#pragma once
#include "TextureSampler.h"
#include "../Rendering/DescriptorPool.h"
#include "Scene/Model.h"  

class Image;
class Device;
class SwapChain;
class CommandPool;
class FrameContext;
class RenderContext;
class ResourceManager final
{
private:
	const std::string TEXTURE_PATH = "resources/textures/viking_room.png";
	const std::string MODEL_PATH = "resources/models/viking_room.obj";

public:
	ResourceManager(Device& device, RenderContext& renderContext, FrameContext& frameContext);
	~ResourceManager() = default;
	void DrawAll(VkCommandBuffer cmd, VkPipelineLayout pipelineLayout, uint32_t frameIndex) const;
	

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

private:
	Device& m_Device;
	CommandPool& m_CommandPool;
	std::vector<std::unique_ptr<Model>> m_Models;
	DescriptorPool& m_DescriptorPool;
	std::shared_ptr<TextureSampler> m_TextureSampler;


	void CreateTextureImage();
	void LoadModel(VkDescriptorSetLayout descriptorSetLayout, const std::string& modelPath, const std::string& texturePath);
};