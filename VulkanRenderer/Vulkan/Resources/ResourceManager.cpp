#include "ResourceManager.h"
#include "../Frame/FrameContext.h"
#include "../Rendering/RenderContext.h"
#include "Scene/MeshLoader.h"
#include <array>
//#include "../Rendering/Pipeline.h"

ResourceManager::ResourceManager(Device& device, RenderContext& renderContext, FrameContext& frameContext)
	: m_Device(device)
	, m_CommandPool(frameContext.GetCommandPool())
    , m_Models()
	, m_DescriptorPool(renderContext.GetDescriptorPool())
	, m_Textures(device, frameContext.GetCommandPool(), renderContext.GetDescriptorPool(), renderContext.GetPipeline().GetMaterialSetLayout())
{

    LoadModel(renderContext.GetPipeline().GetMaterialSetLayout(), MODEL_PATH);

    renderContext.GetDepthPrepass().SetTextureCount(m_Textures.GetTextureCount());
    renderContext.GetDepthPrepass().Build();

    renderContext.GetPipeline().SetTextureCount(m_Textures.GetTextureCount());
    renderContext.GetPipeline().Build();

 
}

void ResourceManager::LoadModel(VkDescriptorSetLayout descriptorSetLayout, const std::string& modelPath, const std::string& texturePath)
{

    m_Models.emplace_back(std::make_unique<Model>(m_Device, m_CommandPool, modelPath, m_Textures, texturePath));
  
}

void ResourceManager::DrawAll(VkCommandBuffer cmd, const Pipeline& pipeline, uint32_t frameIndex) const
{
    for (const auto& model : m_Models)
        model->Draw(cmd, pipeline, frameIndex);
}

void ResourceManager::LoadModel(VkDescriptorSetLayout layout, const std::string& modelPath)
{
    auto primitives = MeshLoader::Load(modelPath);

    for (auto& prim : primitives)
    {
        m_Models.emplace_back(std::make_unique<Model>(m_Device, m_CommandPool, prim.vertices, prim.indices, m_Textures, prim.texturePath));
    }

    m_Textures.Build();
}
