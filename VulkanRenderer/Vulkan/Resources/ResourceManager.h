#pragma once
#include "TextureSampler.h"
#include "../Rendering/DescriptorPool.h"
#include "Scene/Model.h"  
#include "Scene/TextureArray.h"

class Image;
class Device;
class SwapChain;
class CommandPool;
class FrameContext;
class RenderContext;
class Pipeline;

class ResourceManager final
{
private:
	//const std::string TEXTURE_PATH = "resources/textures/viking_room.png";
	//const std::string MODEL_PATH = "resources/models/viking_room.obj";
	const std::string MODEL_PATH = "resources/models/Sponza/Sponza.gltf";

public:
	ResourceManager(Device& device, RenderContext& renderContext, FrameContext& frameContext);
	~ResourceManager() = default;
	void DrawAll(VkCommandBuffer cmd, const Pipeline& pipeline, uint32_t frameIndex) const;
	TextureArray& GetTextureArray() { return m_Textures; };

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
	ResourceManager(ResourceManager&&) = delete;
	ResourceManager& operator=(ResourceManager&&) = delete;

private:
	Device& m_Device;
	CommandPool& m_CommandPool;
	std::vector<std::unique_ptr<Model>> m_Models;
	TextureArray m_Textures;
	DescriptorPool& m_DescriptorPool;

	

	void LoadModel(VkDescriptorSetLayout descriptorSetLayout, const std::string& modelPath, const std::string& texturePath);
	void LoadModel(VkDescriptorSetLayout layout, const std::string& modelPath);
};