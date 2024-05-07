//
// Created by Junhao Wang (@forkercat) on 4/13/24.
//

#include "model.h"

#include "engine/utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace std
{
	template <>
	struct hash<Mapo::Model::Vertex>
	{
		size_t operator()(const Mapo::Model::Vertex& vertex) const
		{
			size_t seed = 0;
			Mapo::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

} // namespace std

namespace Mapo
{
	std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
		return attributeDescriptions;
	}

	Model::Model(VulkanDevice& device, const Builder& builder)
		: m_device(device)
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffers(builder.indices);
	}

	Model::~Model()
	{
	}

	void Model::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_vertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		// TODO: Consider adding a Bind() function in the buffer class.
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_hasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Model::Draw(VkCommandBuffer commandBuffer)
	{
		if (m_hasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, m_vertexCount, 1, 0, 0);
		}
	}

	void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_vertexCount = static_cast<U32>(vertices.size());
		MP_ASSERT(m_vertexCount >= 3, "Failed to create vertex buffer. Vertex count must be at least 3!");

		U32 vertexSize = sizeof(vertices[0]);
		VkDeviceSize bufferSize = vertexSize * m_vertexCount;

		// Create staging buffer and it will be auto deleted.
		VulkanBuffer stagingBuffer{
			m_device,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());

		// Create vertex buffer.
		m_vertexBuffer = MakeUnique<VulkanBuffer>(
			m_device,
			vertexSize,
			m_vertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Copy staging to vertex buffer.
		m_device.CopyBuffer(stagingBuffer.GetBuffer(), m_vertexBuffer->GetBuffer(), bufferSize);
	}

	void Model::CreateIndexBuffers(const std::vector<U32>& indices)
	{
		m_indexCount = static_cast<U32>(indices.size());
		m_hasIndexBuffer = m_indexCount > 0;

		if (!m_hasIndexBuffer)
		{
			return;
		}

		U32 indexSize = sizeof(indices[0]);
		VkDeviceSize bufferSize = indexSize * m_indexCount;

		// Create staging buffer and it will be auto deleted.
		VulkanBuffer stagingBuffer{
			m_device,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		// Create index buffer.
		m_indexBuffer = MakeUnique<VulkanBuffer>(
			m_device,
			indexSize,
			m_indexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// Copy staging to vertex buffer.
		m_device.CopyBuffer(stagingBuffer.GetBuffer(), m_indexBuffer->GetBuffer(), bufferSize);
	}

	UniqueRef<Model> Model::CreateCubeModel(VulkanDevice& device, Vector3 offset)
	{
		// temporary helper function, creates a 1x1x1 cube centered at offset
		Builder modelBuilder{};

		modelBuilder.vertices = {
			// left face (white)
			{ { -.5f, -.5f, -.5f }, { .9f, .9f, .9f } },
			{ { -.5f, .5f, .5f }, { .9f, .9f, .9f } },
			{ { -.5f, -.5f, .5f }, { .9f, .9f, .9f } },
			{ { -.5f, .5f, -.5f }, { .9f, .9f, .9f } },

			// right face (yellow)
			{ { .5f, -.5f, -.5f }, { .8f, .8f, .1f } },
			{ { .5f, .5f, .5f }, { .8f, .8f, .1f } },
			{ { .5f, -.5f, .5f }, { .8f, .8f, .1f } },
			{ { .5f, .5f, -.5f }, { .8f, .8f, .1f } },

			// top face (orange, remember y-axis points down)
			{ { -.5f, -.5f, -.5f }, { .9f, .6f, .1f } },
			{ { .5f, -.5f, .5f }, { .9f, .6f, .1f } },
			{ { -.5f, -.5f, .5f }, { .9f, .6f, .1f } },
			{ { .5f, -.5f, -.5f }, { .9f, .6f, .1f } },

			// bottom face (red)
			{ { -.5f, .5f, -.5f }, { .8f, .1f, .1f } },
			{ { .5f, .5f, .5f }, { .8f, .1f, .1f } },
			{ { -.5f, .5f, .5f }, { .8f, .1f, .1f } },
			{ { .5f, .5f, -.5f }, { .8f, .1f, .1f } },

			// nose face (blue)
			{ { -.5f, -.5f, 0.5f }, { .1f, .1f, .8f } },
			{ { .5f, .5f, 0.5f }, { .1f, .1f, .8f } },
			{ { -.5f, .5f, 0.5f }, { .1f, .1f, .8f } },
			{ { .5f, -.5f, 0.5f }, { .1f, .1f, .8f } },

			// tail face (green)
			{ { -.5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
			{ { .5f, .5f, -0.5f }, { .1f, .8f, .1f } },
			{ { -.5f, .5f, -0.5f }, { .1f, .8f, .1f } },
			{ { .5f, -.5f, -0.5f }, { .1f, .8f, .1f } },
		};

		for (auto& v : modelBuilder.vertices)
		{
			v.position += offset;
		}

		modelBuilder.indices = {
			0, 1, 2, 0, 3, 1, 4, 5, 6, 4, 7, 5, 8, 9, 10, 8, 11, 9, 12, 13,
			14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21
		};

		return MakeUnique<Model>(device, modelBuilder);
	}

	UniqueRef<Model> Model::CreateModelFromFile(VulkanDevice& device, const std::string& filepath)
	{
		Builder builder{};
		builder.LoadModel(filepath);
		MP_PRINT("Vertex count: %zu", builder.vertices.size());
		return MakeUnique<Model>(device, builder);
	}

	void Model::Builder::LoadModel(const std::string& filepath)
	{
		using tinyobj::attrib_t;
		using tinyobj::index_t;
		using tinyobj::material_t;
		using tinyobj::shape_t;

		attrib_t attrib;
		std::vector<shape_t> shapes;
		std::vector<material_t> materials;
		std::string warn, error;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.c_str()))
		{
			MP_ASSERT(false, "Failed to load model: %s", filepath.c_str());
		}

		vertices.clear();
		indices.clear();

		// [Vertex: Index]
		std::unordered_map<Vertex, U32> uniqueVertices{};

		for (const shape_t& shape : shapes)
		{
			for (const index_t& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				// Encounter new vertex. Only add to vertices when it is not yet added.
				if (uniqueVertices.count(vertex) == 0)
				{
					U32 vertexIndex = static_cast<U32>(vertices.size());
					uniqueVertices[vertex] = vertexIndex;
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

} // namespace Mapo
