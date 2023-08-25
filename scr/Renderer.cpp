#include "../cubecraft/Renderer.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
	Renderer::Renderer() {
		InitCommandPool();
		allocCmdBuffer();
		createSems();
		createFence();
	}
	Renderer::~Renderer() {
		auto& device = Context::GetInstance().device;
		device.freeCommandBuffers(cmdPool_, cmdBufffer_);
		device.destroyCommandPool(cmdPool_);
		device.destroySemaphore(imageAvaliable_);
		device.destroySemaphore(imageDrawFinish_);
		device.destroyFence(cmdAvaliableFence_);
	}

	void Renderer::InitCommandPool() {
		vk::CommandPoolCreateInfo createInfo;
		createInfo.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
		cmdPool_ = Context::GetInstance().device.createCommandPool(createInfo);
	}
	void Renderer::allocCmdBuffer() {
		vk::CommandBufferAllocateInfo allocInfo;
		allocInfo.setCommandPool(cmdPool_);
		allocInfo.setCommandBufferCount(1);
		allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);

		cmdBufffer_ = Context::GetInstance().device.allocateCommandBuffers(allocInfo)[0];
	}
	void Renderer::Render() {
		auto& device = Context::GetInstance().device;
		auto& swapChain = Context::GetInstance().swapChain;
		auto& renderProcess = Context::GetInstance().renderProcess;

		auto result = device.acquireNextImageKHR(
			swapChain->swapchain, 
			std::numeric_limits<uint64_t>::max()
		);

		if (result.result != vk::Result::eSuccess) {
			std::cout << "failed to acquire next image\n";
		}
		auto imageIndex = result.value;

		cmdBufffer_.reset();

		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		cmdBufffer_.begin(beginInfo); 
		{
			cmdBufffer_.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->pipeline);
			vk::RenderPassBeginInfo renderPassBegin;
			vk::Rect2D area;
			vk::ClearValue clearValue;
			clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{ 0.1, 0.1, 0.1, 1.0 }));
			area.setExtent({ 0, 0 });
			area.setExtent({ WIDTH, HEIGHT });
			renderPassBegin.setRenderPass(renderProcess->renderPass);
			renderPassBegin.setRenderArea(area);
			renderPassBegin.setFramebuffer(swapChain->framebuffers[imageIndex]);
			renderPassBegin.setClearValues(clearValue);
			cmdBufffer_.beginRenderPass(renderPassBegin, {});
			{
				cmdBufffer_.draw(3, 1, 0, 0);
			}
			cmdBufffer_.endRenderPass();
		}
		cmdBufffer_.end();

		vk::SubmitInfo submitInfo;
		submitInfo.setCommandBuffers(cmdBufffer_);
		Context::GetInstance().graphicsQueue.submit(submitInfo, cmdAvaliableFence_);

		vk::PresentInfoKHR present;
		present.setImageIndices(imageIndex);
		present.setSwapchains(swapChain->swapchain);

		Context::GetInstance().presentQueue.presentKHR(present);
		
		device.waitForFences(cmdAvaliableFence_, true, std::numeric_limits<uint64_t>::max());
		device.resetFences(cmdAvaliableFence_);
	}
	void Renderer::createSems() {
		auto& device = Context::GetInstance().device;
		vk::SemaphoreCreateInfo createInfo;

		imageAvaliable_ = device.createSemaphore(createInfo);
		imageDrawFinish_ = device.createSemaphore(createInfo);
	}
	void Renderer::createFence() {
		auto& device = Context::GetInstance().device;
		vk::FenceCreateInfo createInfo;

		cmdAvaliableFence_ = device.createFence(createInfo);
	}
}