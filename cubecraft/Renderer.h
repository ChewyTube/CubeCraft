#pragma once

#include <vulkan/vulkan.hpp>
//#include "Math.h"

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

		vk::Semaphore imageAvaliable_;
		vk::Semaphore imageDrawFinish_;
		vk::Fence cmdAvaliableFence_;

		void createSems();
		void createFence();

		void createCmdBuffers();
	};
}