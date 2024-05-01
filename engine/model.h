//
// Created by Junhao Wang (@forkercat) on 4/13/24.
//

#pragma once

#include "core/core.h"

#include "engine/vulkan_device.h"
#include "engine/vulkan_buffer.h"

#include <vector>

namespace Mapo
{
	class Model
	{
	public:
		struct Vertex
		{
			Vector3 position;
			Vector3 color;
			Vector3 normal;
			Vector2 uv;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<U32> indices{};

			void LoadModel(const std::string& filepath);
		};

		Model(VulkanDevice& device, const Builder& builder);
		~Model();

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		static UniqueRef<Model> CreateCubeModel(VulkanDevice& device, Vector3 offset);
		static UniqueRef<Model> CreateModelFromFile(VulkanDevice& device, const std::string& filepath);

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<U32>& indices);

	private:
		VulkanDevice& m_device;

		UniqueRef<VulkanBuffer> m_vertexBuffer;
		U32 m_vertexCount;

		bool m_hasIndexBuffer = false;
		UniqueRef<VulkanBuffer> m_indexBuffer;
		U32 m_indexCount;
	};

} // namespace Mapo
