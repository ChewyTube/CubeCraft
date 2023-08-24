#pragma once 
#include "Context.h"

namespace cubecraft {
	static void Init(GLFWwindow* window) {
		Context context(0);//实例化
		context.Init(window);//创建类instance 以及Vulkan实例
		context.GetInstance().InitVulkan(window);//初始化Vulkan
		context.GetInstance().swapChain.reset(new SwapChain());
		context.GetInstance().createShader(ReadWholeFile(vertPath), ReadWholeFile(fragPath));
		context.GetInstance().renderProcess.reset(new RenderProcess());
		context.GetInstance().renderProcess->InitRenderPass();
		context.GetInstance().renderProcess->InitLayout();
		context.GetInstance().swapChain->createFrameBuffers();
		context.GetInstance().renderProcess->InitPipeline();

	}
	static void Quit() {
		Context::GetInstance().
		//Context::GetInstance().renderProcess->DestroyPipeline();
		Context::GetInstance().swapChain->DestroySwapChain();
		Context::Quit();
	}
}