#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <optional>
#include <memory>
#include <iostream>
#include "Shader.h"
#include "RenderProcess.h"
#include "../scr/Utils/ReadWholeFile.h"

namespace cubecraft{
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;

	class Context {
	public:
		void Init();
		void InitWindow();
		void InitVulkan();
		void MainLoop();
		void QuitVulkan();
		void QuitWindow();

		static Context GetInstance();
		static std::unique_ptr<Context> instance_;

		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;
		vk::Device device;//逻辑设备

		struct QueueFamilyIndecis
		{
			std::optional<uint32_t> graphicsQueue;
			std::optional<uint32_t> presentQueue;

			bool IsComplete() {
				return graphicsQueue.has_value() && presentQueue.has_value();
			}
		};
	private:
		RenderProcess renderProcess;

		struct SwapChainInfo
		{
			vk::Extent2D imageExtent;
			vk::SurfaceFormatKHR imageFormat;
			vk::SurfaceTransformFlagsKHR transform;
			vk::PresentModeKHR present;
			uint32_t imageCount;
		};

		GLFWwindow* window;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		bool showAvailableExtensions = true;
		bool showAvailableLayers = true;
		bool showPickedGPU = true;

		vk::Instance instance;//Vulkan实例
		vk::PhysicalDevice phyDevice;//物理设备
		vk::Queue graphicsQueue;//图形队列
		vk::Queue presentQueue;//命令队列
		vk::SwapchainKHR swapchain;//交换链
		VkSurfaceKHR surface;//表面
		QueueFamilyIndecis queueFamilyIndecis;
		SwapChainInfo swapChainInfo;

		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
		std::vector<vk::Image> images;
		std::vector<vk::ImageView> imageViews;

		const std::string vertPath = "D:\\Vulkan\\Program\\CubeCraft\\shaders\\vert.spv";
		const std::string fragPath = "D:\\Vulkan\\Program\\CubeCraft\\shaders\\frag.spv";

		//------------------------Context.cpp------------------------
		void createInstance();
		void pickupPhyicalDevice();
		void createDevice();
		void createSurface();
		void queryQueueFamilyIndecis();
		void getQueues();

		//------------------------Layers.cpp------------------------
		void getLayers();

		//------------------------SwapChain.cpp------------------------
		void createSwapChain();
		void querySwapChainInfo();
		void getImages();
		void createImageViews();

		//------------------------Shader.cpp------------------------
		void createShader(const std::string& vertSource, const std::string& fragSource);
		void destroyShader();
	};
}