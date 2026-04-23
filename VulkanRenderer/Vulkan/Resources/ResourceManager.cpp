#include "ResourceManager.h"
#include "../Frame/FrameContext.h"
#include "../Rendering/RenderContext.h"

#include <array>

ResourceManager::ResourceManager(Device& device, RenderContext& renderContext, FrameContext& frameContext)
	: m_Device(device)
	, m_CommandPool(frameContext.GetCommandPool())
    , m_Models()
	, m_DescriptorPool(renderContext.GetDescriptorPool())
	, m_TextureSampler(std::make_shared<TextureSampler>(device))
{
    //CreateTextureImage();
    LoadModel(renderContext.GetPipeline().GetMaterialSetLayout(), MODEL_PATH, TEXTURE_PATH);
}

void ResourceManager::LoadModel(VkDescriptorSetLayout descriptorSetLayout, const std::string& modelPath, const std::string& texturePath)
{

    m_Models.emplace_back(std::make_unique<Model>(m_Device, m_CommandPool, m_DescriptorPool, descriptorSetLayout, modelPath, texturePath, m_TextureSampler));
  
}

void ResourceManager::DrawAll(VkCommandBuffer cmd, VkPipelineLayout pipelineLayout, uint32_t frameIndex) const
{
    for (const auto& model : m_Models)
        model->Draw(cmd, pipelineLayout, frameIndex);
}

