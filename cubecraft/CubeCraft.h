#pragma once 
#include <iostream>
#include "Context.h"
#include "Math.h"

namespace cubecraft {
	static void Init(GLFWwindow* window) {
		Context::Init(window);//创建类instance 以及Vulkan实例
		auto& ctx = Context::Instance();
		ctx.initSwapchain();
		ctx.initShaderModules();
		ctx.initRenderProcess();
		ctx.initGraphicsPipeline();
		ctx.swapChain->createFrameBuffers();
		ctx.initCommandPool();
		ctx.initSampler();
		
		int maxFlightCount = 2;
		DescriptorSetManager::Init(maxFlightCount);

		ctx.initRenderer();
	}
	static void Quit() {
		Context::Instance().device.waitIdle();
		Context::Instance().renderer.reset();
		Context::Instance().swapChain->DestroySwapChain();

		Context::Instance().shader.reset();
		Context::Instance().commandManager.reset();
		Context::Instance().renderProcess.reset();
		Context::Instance().swapChain.reset();

		Context::Quit();
	}

	Texture* LoadTexture(const std::string& filename) {
		return TextureManager::Instance().Load(filename);
	}

	void DestroyTexture(Texture* texture) {
		TextureManager::Instance().Destroy(texture);
	}
}