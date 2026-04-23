#include "Material.h"
#include "Buffers.h"
#include "Image.h"
#include <cmath>
#include <stdexcept>
#include "DescriptorPool.h"
#include "../CommandPool.h"
#include <array>
#include "../ResourcesUtils.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Material::Material(Device& device, CommandPool& commandPool, DescriptorPool& pool, VkDescriptorSetLayout descriptorSetLayout, const std::string& texturePath, std::shared_ptr<TextureSampler> textureSampler)
    : m_Device(device)
    , m_CommandPool(commandPool)
    , m_TextureImage(nullptr)
    , m_TexturePath(texturePath)
    , m_TextureSampler(textureSampler)
{
    CreateTextureImage();
	m_DescriptorSets = pool.AllocateSets(descriptorSetLayout);
	WriteDescriptorSet();
}

Material::~Material()
{

}

void Material::Bind(VkCommandBuffer commandBuffer, VkPipelineLayout layout, uint32_t frameIndex) const
{

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 1, 1, &m_DescriptorSets[frameIndex], 0, nullptr);
}


void Material::CreateTextureImage()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(m_TexturePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }
    Buffer stagingBuffer(m_Device, m_CommandPool, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.Upload(pixels, imageSize);


    stbi_image_free(pixels);

    const uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    m_TextureImage = std::make_unique<Image>(m_Device, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

    m_TextureImage->TransitionImageLayout(m_CommandPool, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_NONE, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    m_TextureImage->CopyBufferToImage(m_CommandPool, stagingBuffer.GetBuffer());

    if (mipLevels != 1)
    {
        m_TextureImage->GenerateMipmaps(m_CommandPool);
    }
    else
    {
        m_TextureImage->TransitionImageLayout(m_CommandPool, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

void Material::WriteDescriptorSet()
{
    // No UBO here at all — texture only
    for (size_t i = 0; i < m_DescriptorSets.size(); i++)
    {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_TextureImage->m_ImageView;
		imageInfo.sampler = m_TextureSampler->GetSampler();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_DescriptorSets[i];
        write.dstBinding = 0;     
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_Device.GetDevice(), 1, &write, 0, nullptr);
    }
}


