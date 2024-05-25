//
// Created by Junhao Wang (@forkercat) on 4/13/24.
//

#pragma once

#include "core/core.h"

#include <vulkan/vulkan.h>

namespace Mapo
{
	class Device;
	class Buffer;

	class Model
	{
	public:
		struct Vertex
		{
			Vector3 position;
			Vector3 color;
			Vector3 normal;
			Vector2 uv;

			static std::vector<VkVertexInputBindingDescription>	  GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<U32>	indices{};

			String modelName{};

			void LoadModel(const String& filepath);
		};

		virtual ~Model();

		Model(const Builder& builder);

		Model(const Model&) = delete;
		Model& operator=(const Model&) = delete;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		const String& GetModelName() const { return m_modelName; }
		U32			  GetVertexCount() const { return m_vertexCount; }
		U32			  GetIndexCount() const { return m_indexCount; }

		static UniqueRef<Model> CreateCubeModel();
		static UniqueRef<Model> CreateModelFromFile(const String& filepath);

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<U32>& indices);

	private:
		Device& m_device;

		UniqueRef<Buffer> m_vertexBuffer;
		U32				  m_vertexCount;

		bool			  m_hasIndexBuffer = false;
		UniqueRef<Buffer> m_indexBuffer;
		U32				  m_indexCount;

		String m_modelName{};
	};

} // namespace Mapo
