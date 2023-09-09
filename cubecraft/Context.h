#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <optional>
#include <memory>
#include <iostream>
#include "Shader.h"
#include "RenderProcess.h"
#include "SwapChain.h"
#include "Renderer.h"
#include "CommandManager.h"

#include "Camera.h"

#include "Utils/ReadWholeFile.h"

namespace cubecraft{

	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	const std::string vertPath = "D:\\Vulkan\\Program\\CubeCraft\\shaders\\vert.spv";
	const std::string fragPath = "D:\\Vulkan\\Program\\CubeCraft\\shaders\\frag.spv";


	class Context final{
	public:
		struct QueueFamilyIndecis
		{
			std::optional<uint32_t> graphicsQueue;
			std::optional<uint32_t> presentQueue;

			bool IsComplete() {
				return graphicsQueue.has_value() && presentQueue.has_value();
			}
		};

		static void Init(GLFWwindow* window);
		static void Quit();
		static Context& Instance() {
			//std::cout << instance_;
			//if(!instance_)instance_ = new (std::nothrow) Context();
			assert(instance_);
			/*
			if (instance_ == nullptr) {
				throw std::runtime_error("初始化失败");
				//instance_ = new (std::nothrow) Context();
			}
			*/
			return *instance_;
		}

		~Context();

		void InitVulkan(GLFWwindow* window);

		Camera camera;

		vk::Instance instance;//Vulkan实例
		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;
		vk::Device device;//逻辑设备
		VkSurfaceKHR surface;//表面
		vk::PhysicalDevice phyDevice;//物理设备
		QueueFamilyIndecis queueFamilyIndecis;
		vk::Queue graphicsQueue;//图形队列
		vk::Queue presentQueue;//命令队列

		std::unique_ptr<SwapChain> swapChain;
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<RenderProcess> renderProcess;
		std::unique_ptr<CommandManager> commandManager;
		std::unique_ptr<Shader> shader;

		bool fiestTime = true;

		void initRenderProcess();
		void initSwapchain();
		void initGraphicsPipeline();
		void initCommandPool();
		void initShaderModules();
		void initRenderer();
		//void initSampler();
		//void getSurface();

		Renderer* GetRenderer() {
			return Context::Instance().renderer.get();
		}

	private:
		static std::unique_ptr<Context> instance_;

		//RenderProcess renderProcess;

		Context(GLFWwindow* window);


		const std::vector<const char*> validationLayers = {
			"VK_LAYER_LUNARG_standard_validation"
		};

		bool showAvailableExtensions = true;
		bool showAvailableLayers = true;
		bool showPickedGPU = true;

		

		std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };

		

		//------------------------Context.cpp------------------------
		void createInstance();
		void pickupPhyicalDevice();
		void createDevice();
		void createSurface(GLFWwindow* window);
		void queryQueueFamilyIndecis();
		void getQueues();

		//------------------------Layers.cpp------------------------
		void getLayers();	
	};
}