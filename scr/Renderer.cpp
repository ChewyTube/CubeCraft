#include "../cubecraft/Renderer.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
	Renderer::Renderer(int maxFlightCount) : maxFlightCount_(maxFlightCount), curFrame_(0) {
		createSems();
		createFence();
        createCmdBuffers();
	}
	Renderer::~Renderer() {
		auto& device = Context::GetInstance().device;
		for (auto& sem : imageAvaliableSems_) {
			device.destroySemaphore(sem);
		}
		for (auto& sem : renderFinishSems_) {
			device.destroySemaphore(sem);
		}
		for (auto& fence : fences_) {
			device.destroyFence(fence);
		}
	}

    void Renderer::StartRender() {
        auto& ctx = Context::GetInstance();
        auto& device = ctx.device;
        if (device.waitForFences(fences_[curFrame_], true, std::numeric_limits<std::uint64_t>::max()) != vk::Result::eSuccess) {
            throw std::runtime_error("wait for fence failed");
        }
        device.resetFences(fences_[curFrame_]);

        auto& swapchain = ctx.swapChain;
        auto resultValue = device.acquireNextImageKHR(swapchain->swapchain, std::numeric_limits<std::uint64_t>::max(), imageAvaliableSems_[curFrame_], nullptr);
        if (resultValue.result != vk::Result::eSuccess) {
            throw std::runtime_error("wait for image in swapchain failed");
        }
        imageIndex_ = resultValue.value;

        auto& cmdMgr = ctx.commandManager;
        auto& cmd = cmdBufs_[curFrame_];
        cmd.reset();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmd.begin(beginInfo);
        vk::ClearValue clearValue;
        clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.1, 0.1, 0.1, 1}));
        vk::RenderPassBeginInfo renderPassBegin;
        renderPassBegin.setRenderPass(ctx.renderProcess->renderPass)
            .setFramebuffer(swapchain->framebuffers[imageIndex_])
            .setClearValues(clearValue)
            .setRenderArea(vk::Rect2D({}, swapchain->GetExtent()));
        cmd.beginRenderPass(&renderPassBegin, vk::SubpassContents::eInline);
    }
    void Renderer::EndRender() {
        auto& ctx = Context::GetInstance();
        auto& swapchain = ctx.swapChain;
        auto& cmd = cmdBufs_[curFrame_];
        cmd.endRenderPass();
        cmd.end();

        vk::SubmitInfo submit;
        vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
        submit.setCommandBuffers(cmd)
            .setWaitSemaphores(imageAvaliableSems_[curFrame_])
            .setWaitDstStageMask(flags)
            .setSignalSemaphores(renderFinishSems_[curFrame_]);
        ctx.graphicsQueue.submit(submit, fences_[curFrame_]);

        vk::PresentInfoKHR presentInfo;
        presentInfo.setWaitSemaphores(renderFinishSems_[curFrame_])
            .setSwapchains(swapchain->swapchain)
            .setImageIndices(imageIndex_);
        if (ctx.presentQueue.presentKHR(presentInfo) != vk::Result::eSuccess) {
            throw std::runtime_error("present queue execute failed");
        }

        curFrame_ = (curFrame_ + 1) % maxFlightCount_;
    }
    void Renderer::render() {
        auto& cmd = cmdBufs_[curFrame_];
        auto& renderProcess = Context::GetInstance().renderProcess;

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->graphicsPipelineWithTriangleTopology);
        cmd.draw(3, 1, 0, 0);
    }
    /*
    void Renderer::DrawTexture(const Rect& rect, Texture& texture) {
        auto& ctx = Context::GetInstance();
        auto& device = ctx.device;
        auto& cmd = cmdBufs_[curFrame_];
        vk::DeviceSize offset = 0;

        bufferRectData();

        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, ctx.renderProcess->graphicsPipelineWithTriangleTopology);
        cmd.bindVertexBuffers(0, rectVerticesBuffer_->buffer, offset);
        cmd.bindIndexBuffer(rectIndicesBuffer_->buffer, 0, vk::IndexType::eUint32);

        auto& layout = Context::Instance().renderProcess->layout;
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
            layout,
            0, { descriptorSets_[curFrame_].set, texture.set.set }, {});
        auto model = Mat4::CreateTranslate(rect.position).Mul(Mat4::CreateScale(rect.size));
        cmd.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Mat4), model.GetData());
        cmd.pushConstants(layout, vk::ShaderStageFlagBits::eFragment, sizeof(Mat4), sizeof(Color), &drawColor_);
        cmd.drawIndexed(6, 1, 0, 0, 0);
    }
    */
    
	void Renderer::createSems() {
		auto& device = Context::GetInstance().device;
		vk::SemaphoreCreateInfo info;

		imageAvaliableSems_.resize(maxFlightCount_);
		renderFinishSems_.resize(maxFlightCount_);

		for (auto& sem : imageAvaliableSems_) {
			sem = device.createSemaphore(info);
		}

		for (auto& sem : renderFinishSems_) {
			sem = device.createSemaphore(info);
		}
	}
	void Renderer::createFence() {
		fences_.resize(maxFlightCount_, nullptr);

		for (auto& fence : fences_) {
			vk::FenceCreateInfo fenceCreateInfo;
			fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
			fence = Context::GetInstance().device.createFence(fenceCreateInfo);
		}
	}
    void Renderer::createCmdBuffers() {
        cmdBufs_.resize(maxFlightCount_);

        for (auto& cmd : cmdBufs_) {
            cmd = Context::GetInstance().commandManager->CreateOneCommandBuffer();
        }
    }
}