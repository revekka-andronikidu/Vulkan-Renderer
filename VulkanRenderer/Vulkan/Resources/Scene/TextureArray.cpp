#include "TextureArray.h"
#include <cmath>
#include <array>
#include <stdexcept>
#include "../Buffer.h"
#include "../Image.h"
#include "../../Rendering/DescriptorPool.h"
#include "../../Frame/CommandPool.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


TextureArray::TextureArray(Device& device, CommandPool& commandPool, DescriptorPool& pool, VkDescriptorSetLayout descriptorSetLayout)
    : m_Device(device)
    , m_CommandPool(commandPool)
	, m_Pool(pool)
	, m_Layout(descriptorSetLayout)
    , m_TextureSampler(std::make_shared<TextureSampler>(device))
	, m_TextureImages()
{

	//m_DescriptorSets = pool.AllocateSets(descriptorSetLayout);
	//WriteDescriptorSets();
}

void TextureArray::Build()
{
    m_DescriptorSets = m_Pool.AllocateSets(m_Layout);
    WriteDescriptorSets();
}

uint32_t TextureArray::AddTexture(const std::string& path)
{
    uint32_t index = static_cast<uint32_t>(m_TextureImages.size());
    CreateTextureImage(path);
    return index;  
}

TextureArray::~TextureArray()
{
}

void TextureArray::Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const
{
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, &m_DescriptorSets[frameIndex], 0, nullptr);
}


void TextureArray::CreateTextureImage(const std::string& path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    Buffer stagingBuffer(m_Device, m_CommandPool, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Upload(pixels, imageSize);


    stbi_image_free(pixels);

    const uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    m_TextureImages.push_back(std::make_unique<Image>(m_Device, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT));


    m_TextureImages[m_TextureImages.size() - 1]->TransitionImageLayout(m_CommandPool, VK_PIPELINE_STAGE_2_NONE, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_NONE, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    m_TextureImages[m_TextureImages.size() - 1]->CopyBufferToImage(m_CommandPool, stagingBuffer.GetBuffer());

    if (mipLevels != 1)
    {
        m_TextureImages[m_TextureImages.size() - 1]->GenerateMipmaps(m_CommandPool);
    }
    else
    {
        m_TextureImages[m_TextureImages.size() - 1]->TransitionImageLayout(m_CommandPool, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_ACCESS_2_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

void TextureArray::WriteDescriptorSets()
{
    VkDescriptorImageInfo samplerInfo{};
    samplerInfo.sampler = m_TextureSampler->GetSampler();

    std::vector<VkDescriptorImageInfo> imageInfos(m_TextureImages.size());
    for (size_t i = 0; i < m_TextureImages.size(); i++)
    {
        imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfos[i].imageView = m_TextureImages[i]->m_ImageView;
        imageInfos[i].sampler = VK_NULL_HANDLE;  // sampler is separate
    }

    for (size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
        std::array<VkWriteDescriptorSet, 2> writes{};

        writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[0].dstSet = m_DescriptorSets[i];
        writes[0].dstBinding = 0;
        writes[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        writes[0].descriptorCount = 1;
        writes[0].pImageInfo = &samplerInfo;

        writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[1].dstSet = m_DescriptorSets[i];
        writes[1].dstBinding = 1;
        writes[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[1].descriptorCount = static_cast<uint32_t>(imageInfos.size());
        writes[1].pImageInfo = imageInfos.data();

        vkUpdateDescriptorSets(m_Device.GetDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    }
}


