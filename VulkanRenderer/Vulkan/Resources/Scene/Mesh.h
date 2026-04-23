#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include <memory>
#include "../Vertex.h"

class CommandPool;
class Buffer;
class Device;
class Mesh final
{
public:
	Mesh(Device& device, CommandPool& commandPool, const std::string& path);
	~Mesh();

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;

	std::vector<Vertex> GetVertices() const { return m_Vertices; }
	std::vector<uint32_t> GetIndices() const { return m_Indices; }

	void Bind(VkCommandBuffer commandBuffer) const;
	void Draw(VkCommandBuffer commandBuffer) const;

	
private:
	Device& m_Device;
	CommandPool& m_CommandPool;
	const std::string& m_Path;
	std::vector<Vertex> m_Vertices;
	std::vector<uint32_t> m_Indices;

	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;

	void LoadMesh();
	void CreateVertexBuffer();
	void CreateIndexBuffer();
};