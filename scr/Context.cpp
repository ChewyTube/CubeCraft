#include "../cubecraft/Context.h"
#include <iostream>
#include <vector>

namespace cubecraft {
	std::unique_ptr<Context> Context::instance_ = nullptr;

	void Context::Init() {
		instance_.reset(new Context);
	}
	void Context::InitWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	}
	void Context::InitVulkan() {
		createInstance();
		if (showAvailableLayers)getLayers();
		pickupPhyicalDevice();
		createSurface();
		queryQueueFamilyIndecis();
		createDevice();
		createShader(ReadWholeFile(vertPath), ReadWholeFile(fragPath));
		renderProcess.InitPipeline();
		getQueues();
	}
	void Context::MainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}
	void Context::QuitVulkan() {
		destroyShader();
		for (auto& view : imageViews) {
			device.destroyImageView(view);
		}
		device.destroySwapchainKHR(swapchain);
		device.destroy();
		vkDestroySurfaceKHR(instance, surface, nullptr);
		instance.destroy();
	}
	void Context::QuitWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Context Context::GetInstance() {
		//std::cout << instance_;
		return *instance_;
	}

	void Context::createInstance() {
		vk::ApplicationInfo appInfo;
		vk::InstanceCreateInfo createInfo;

		auto extensions = vk::enumerateInstanceExtensionProperties();
		if (showAvailableExtensions) {
			std::cout << "������չ��" << std::endl;
			for (auto extension : extensions) {
				std::cout << "\t" << extension.extensionName << std::endl;
			}
		}

		uint32_t glfwExtensionCount = 0;
		auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		appInfo.setPApplicationName("CubeCraft")
			.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
			.setPEngineName("No Engine")
			.setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
			.setApiVersion(VK_API_VERSION_1_3);
		createInfo.setPApplicationInfo(&appInfo)
			.setEnabledExtensionCount(glfwExtensionCount)
			.setPpEnabledExtensionNames(glfwExtensions)
			.setPEnabledLayerNames(layers);
		instance = vk::createInstance(createInfo);
		std::cout << "�ɹ�����ʵ����" << instance << std::endl;
	}
	void Context::pickupPhyicalDevice() {
		auto devices = instance.enumeratePhysicalDevices();
		phyDevice = devices[0];

		if (showPickedGPU) {
			std::cout << "ʹ�õ��Կ���" << phyDevice.getProperties().deviceName << std::endl;
		}
	}
	void Context::createDevice() {
		std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo createInfo;
		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		float priorities = 1.0f;
		if (queueFamilyIndecis.graphicsQueue.value() == queueFamilyIndecis.presentQueue.value()) {
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndecis.graphicsQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
		}
		else {
			vk::DeviceQueueCreateInfo queueCreateInfo;
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndecis.graphicsQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndecis.presentQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
		}
		
		createInfo.setQueueCreateInfos(queueCreateInfos)
			.setPEnabledExtensionNames(extensions);

		device = phyDevice.createDevice(createInfo);
	}
	void Context::queryQueueFamilyIndecis() {
		auto properties = phyDevice.getQueueFamilyProperties();
		for (int i = 0; i < properties.size(); i++) {
			const auto& property = properties[i];
			if (property.queueFlags & vk::QueueFlagBits::eGraphics) {
				queueFamilyIndecis.graphicsQueue = i;
			}
			if (phyDevice.getSurfaceSupportKHR(i, surface)) {
				queueFamilyIndecis.presentQueue = i;
			}
			if (queueFamilyIndecis.IsComplete()) {
				break;
			}
		}
	}
	void Context::getQueues() {
		graphicsQueue = device.getQueue(queueFamilyIndecis.graphicsQueue.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndecis.presentQueue.value(), 0);
	}
	void Context::createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}
}