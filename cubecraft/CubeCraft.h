#pragma once 
#include <iostream>
#include "Context.h"

namespace cubecraft {
	static void Init(GLFWwindow* window) {
		Context::Init(window);//������instance �Լ�Vulkanʵ��
		Context::GetInstance().InitVulkan(window);//��ʼ��Vulkan
		Context::GetInstance().swapChain.reset(new SwapChain());
		Context::GetInstance().createShader(ReadWholeFile(vertPath), ReadWholeFile(fragPath));
		Context::GetInstance().renderProcess.reset(new RenderProcess());
		Context::GetInstance().renderProcess->InitRenderPass();
		Context::GetInstance().renderProcess->InitLayout();
		Context::GetInstance().swapChain->createFrameBuffers();
		Context::GetInstance().renderProcess->InitPipeline();
		Context::GetInstance().renderer.reset(new Renderer());

	}
	static void Quit() {
		Context::GetInstance().
		//Context::GetInstance().renderProcess->DestroyPipeline();
		Context::GetInstance().swapChain->DestroySwapChain();
		Context::Quit();
	}

	inline Renderer& getRenderer() {
		return *Context::GetInstance().renderer;
	}
}