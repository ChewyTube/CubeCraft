#pragma once

#include <vulkan/vulkan.hpp>
#include "Vertex.h"
#include "Buffer.h"
#include "Uniform.h"

namespace cubecraft {
	class Renderer final {
	public:
		Renderer(int maxFlightCount);
		~Renderer();

		void StartRender();
		void EndRender();

		//void DrawTexture(const Rect& rect, Texture& texture);
		void render();
	private:
		int maxFlightCount_;
		int curFrame_;
		uint32_t imageIndex_;
		std::vector<vk::Fence> fences_;
		std::vector<vk::Semaphore> imageAvaliableSems_;
		std::vector<vk::Semaphore> renderFinishSems_;
		std::vector<vk::CommandBuffer> cmdBufs_;

		std::vector<std::unique_ptr<Buffer>> deviceUniformBuffer_;

		std::unique_ptr<Buffer> verticesBuffer_;
		std::unique_ptr<Buffer> indicesBuffer_;

		vk::DescriptorPool descriptorPool_;
		std::vector<vk::DescriptorSet> sets_;

		void createSems();
		void createFence();
		void createCmdBuffers();
		void createBuffers();
		void createUniformBuffers();
		void bufferVertexData();
		void bufferIndicesData();
		void bufferUniformData();

		void bufferData();

		void createDescriptorPool();
		void allocateSets();
		void updateSets();

		void copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset);
		void transformBuffer_To_Device(Buffer& src, Buffer& dst, size_t srcOffset, size_t dstOffset, size_t size);
	};
}