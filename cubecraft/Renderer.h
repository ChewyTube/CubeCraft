#pragma once

#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class Renderer final {
	public:
		Renderer();
		~Renderer();

		void Render();
	private:
		vk::CommandPool cmdPool_;
		vk::CommandBuffer cmdBufffer_;

		vk::Semaphore imageAvaliable_;
		vk::Semaphore imageDrawFinish_;
		vk::Fence cmdAvaliableFence_;

		void InitCommandPool();
		void allocCmdBuffer();
		void createSems();
		void createFence();
	};
}