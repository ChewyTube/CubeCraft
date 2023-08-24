#pragma once

#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class SwapChain final {
	public:
		SwapChain();
		void DestroySwapChain();

		void queryswapchainInfo();
		void getImages();
		void createImageViews();
		void createFrameBuffers();

		struct swapchainInfo
		{
			vk::Extent2D imageExtent;
			vk::SurfaceFormatKHR imageFormat;
			vk::SurfaceTransformFlagsKHR transform;
			vk::PresentModeKHR present;
			uint32_t imageCount;
		};

		swapchainInfo swapchainInfo;

		vk::SwapchainKHR swapchain;//½»»»Á´
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;
		std::vector<vk::Framebuffer> framebuffers;
	};
}