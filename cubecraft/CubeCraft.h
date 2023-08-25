#pragma once 
#include <iostream>
#include "Context.h"
#include "Math.h"

namespace cubecraft {
	static void Init(GLFWwindow* window) {
		Context::Init(window);//创建类instance 以及Vulkan实例
		auto& ctx = Context::GetInstance();
		ctx.initSwapchain();
		ctx.initShaderModules();
		ctx.initRenderProcess();
		ctx.initGraphicsPipeline();
		ctx.swapChain->createFrameBuffers();
		ctx.initCommandPool();
		ctx.initRenderer();
		/*
		Context::GetInstance().InitVulkan(window);//初始化Vulkan
		Context::GetInstance().swapChain.reset(new SwapChain());
		Context::GetInstance().createShader(ReadWholeFile(vertPath), ReadWholeFile(fragPath));
		Context::GetInstance().renderProcess.reset(new RenderProcess());
		Context::GetInstance().renderProcess->InitRenderPass();
		Context::GetInstance().renderProcess->InitLayout();
		Context::GetInstance().swapChain->createFrameBuffers();
		Context::GetInstance().renderProcess->InitPipeline();
		Context::GetInstance().renderer.reset(new Renderer(2));
		*/
		
		//Context::GetInstance().commandManager.reset(new CommandManager());

	}
	static void Quit() {
		//Context::GetInstance().
		//Context::GetInstance().renderProcess->DestroyPipeline();
		Context::GetInstance().device.waitIdle();
		Context::GetInstance().renderer.reset();
		//Context::GetInstance().renderProcess.reset();
		Context::GetInstance().swapChain->DestroySwapChain();

		Context::GetInstance().shader.reset();
		Context::GetInstance().commandManager.reset();
		Context::GetInstance().renderProcess.reset();
		Context::GetInstance().swapChain.reset();

		Context::Quit();
	}
}