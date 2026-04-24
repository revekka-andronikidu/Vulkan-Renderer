#include "Mesh.h"
#include "../Buffer.h"
#include "MeshLoader.h"


Mesh::Mesh(Device& device, CommandPool& commandPool, const std::string& path)
    : m_Device(device)
    , m_CommandPool(commandPool)
    , m_Path(path)
	, m_VertexBuffer(nullptr)
	, m_IndexBuffer(nullptr)
	, m_Vertices({})
	, m_Indices({})
{
    auto meshData = MeshLoader::Load(path);
    m_Vertices = std::move(meshData[0].vertices);
    m_Indices = std::move(meshData[0].indices);

    CreateVertexBuffer();
    CreateIndexBuffer();
}

Mesh::Mesh(Device& device, CommandPool& commandPool, std::vector<Vertex> vertices, std::vector<uint32_t> indices)
    : m_Device(device)
    , m_CommandPool(commandPool)
    , m_Path("")
    , m_VertexBuffer(nullptr)
    , m_IndexBuffer(nullptr)
    , m_Vertices(std::move(vertices))
    , m_Indices(std::move(indices))
{
    CreateVertexBuffer();
    CreateIndexBuffer();
}

Mesh::~Mesh()
{
}   

void Mesh::CreateVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(Vertex) * m_Vertices.size();

    Buffer stagingBuffer(m_Device, m_CommandPool, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.Upload(m_Vertices.data(), bufferSize);

    m_VertexBuffer = std::make_unique<Buffer>(m_Device, m_CommandPool, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_VertexBuffer->CopyBuffer(stagingBuffer.GetBuffer(), bufferSize);
}

void Mesh::CreateIndexBuffer()
{
    VkDeviceSize bufferSize = sizeof(uint32_t) * m_Indices.size();

    Buffer stagingBuffer(m_Device, m_CommandPool, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    stagingBuffer.Upload(m_Indices.data(), bufferSize);

    m_IndexBuffer = std::make_unique<Buffer>(m_Device, m_CommandPool, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    m_IndexBuffer->CopyBuffer(stagingBuffer.GetBuffer(), bufferSize);
}

void Mesh::Bind(VkCommandBuffer commandBuffer) const
{
    VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

void Mesh::Draw(VkCommandBuffer commandBuffer) const
{
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}