#pragma once

#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class Buffer final {
	public:
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		size_t size;
		size_t requireSize;
		void* map;

		Buffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags memProperty);
		~Buffer();

	private:
		struct MemoryInfo final {
			size_t size;
			uint32_t index;
		};

		void createBuffer(size_t size, vk::BufferUsageFlags usage);
		void allocateMemory(MemoryInfo info);
		void bindinfMemory_To_Buffer();
		MemoryInfo queryMemoryInfo(vk::MemoryPropertyFlags property);
		std::uint32_t queryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag);
	};
}