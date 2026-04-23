#include "Mesh.h"

#define TINYOBJLOADER_DISABLE_FAST_FLOAT
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "../ResourcesUtils.h"
#include <iostream>

#include "Buffer.h"


Mesh::Mesh(Device& device, CommandPool& commandPool, const std::string& path)
    : m_Device(device)
    , m_CommandPool(commandPool)
    , m_Path(path)
	, m_VertexBuffer(nullptr)
	, m_IndexBuffer(nullptr)
	, m_Vertices({})
	, m_Indices({})
{
    LoadMesh();
    CreateVertexBuffer();
    CreateIndexBuffer();
}

Mesh::~Mesh()
{
}   

void Mesh::LoadMesh()
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    std::string war;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &war, &err, m_Path.c_str()))
    {
        std::cerr << war << std::endl;
        throw std::runtime_error(err);
    }

    std::unordered_map<Vertex, uint32_t> uniqueVertices{};

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) {
                uniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
                m_Vertices.push_back(vertex);
            }

            m_Indices.push_back(uniqueVertices[vertex]);
        }
    }
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