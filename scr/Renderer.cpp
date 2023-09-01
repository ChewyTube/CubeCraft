#include "../cubecraft/Renderer.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
    std::array<Vertex, 3> vertices = {
            Vertex{0.5, -0.5, 0.0},
            Vertex{0.5 , 0.5, 0.0},
            Vertex{-0.5, 0.5, 0.0}
    };

    const Uniform uniform{ Color{1, 0, 0} };

	Renderer::Renderer(int maxFlightCount) : maxFlightCount_(maxFlightCount), curFrame_(0) {
        createFence();
        createSems();
        createCmdBuffers();
        createBuffers();
        bufferVertexData();
        createUniformBuffers();
        bufferUniformData();
        createDescriptorPool();
        allocateSets();
        updateSets();
	}
	Renderer::~Renderer() {
		auto& device = Context::Instance().device;
        device.destroyDescriptorPool(descriptorPool_);
        hostVertexBuffer_.reset();
        deviceVertexBuffer_.reset();
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
        auto& ctx = Context::Instance();
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
        auto& ctx = Context::Instance();
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
        auto& renderProcess = Context::Instance().renderProcess;
        
        vk::DeviceSize offset = 0;
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Context::Instance().renderProcess->layout, 0, sets_[curFrame_], {});
        cmd.bindVertexBuffers(0, hostVertexBuffer_->buffer, offset);
        
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->graphicsPipelineWithTriangleTopology);
        cmd.draw(3, 1, 0, 0);
    }
    
	void Renderer::createSems() {
		auto& device = Context::Instance().device;
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
			fence = Context::Instance().device.createFence(fenceCreateInfo);
		}
	}
    void Renderer::createCmdBuffers() {
        cmdBufs_.resize(maxFlightCount_);

        for (auto& cmd : cmdBufs_) {
            cmd = Context::Instance().commandManager->CreateOneCommandBuffer();
        }
    }

    void Renderer::createBuffers(){
        hostVertexBuffer_.reset(
            new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
            (size_t)sizeof(vertices),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        ));
        deviceVertexBuffer_.reset(
            new Buffer(vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
            (size_t)sizeof(vertices),
            vk::MemoryPropertyFlagBits::eDeviceLocal
        ));

        indicesBuffer_.reset(new Buffer(
            vk::BufferUsageFlagBits::eIndexBuffer,
            sizeof(float) * 6,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        ));
    }
    void Renderer::bufferVertexData(){
        void* ptr = Context::Instance().device.mapMemory(hostVertexBuffer_->memory, 0, hostVertexBuffer_->size);
        memcpy(ptr, vertices.data(), sizeof(vertices));
        Context::Instance().device.unmapMemory(hostVertexBuffer_->memory);

        copyBuffer(hostVertexBuffer_->buffer, deviceVertexBuffer_->buffer,
            hostVertexBuffer_->size, 0, 0);
    }
    void Renderer::createUniformBuffers() {
        hostUniformBuffer_.resize(maxFlightCount_);
        deviceUniformBuffer_.resize(maxFlightCount_);

        for (auto& buffer : hostUniformBuffer_) {
            buffer.reset(new Buffer(
                vk::BufferUsageFlagBits::eTransferSrc,
                sizeof(uniform),
                vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible));
        }

        for (auto& buffer : deviceUniformBuffer_) {
            buffer.reset(new Buffer(
                vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                sizeof(uniform),
                vk::MemoryPropertyFlagBits::eDeviceLocal));
        }
    }
    void Renderer::bufferUniformData() {
        for (int i = 0; i < hostUniformBuffer_.size(); i++) {
            auto& buffer = hostUniformBuffer_[i];
            void* ptr = Context::Instance().device.mapMemory(buffer->memory, 0, buffer->size);
            memcpy(ptr, &uniform, sizeof(uniform));
            Context::Instance().device.unmapMemory(buffer->memory);

            copyBuffer(buffer->buffer, deviceUniformBuffer_[i]->buffer, buffer->size, 0, 0);
        }
    }

    void Renderer::createDescriptorPool() {
        vk::DescriptorPoolCreateInfo createInfo; 
        vk::DescriptorPoolSize poolSize;

        poolSize.setType(vk::DescriptorType::eUniformBuffer);
        poolSize.setDescriptorCount(maxFlightCount_);

        createInfo.setMaxSets(maxFlightCount_);
        createInfo.setPoolSizes(poolSize);

        descriptorPool_ = Context::Instance().device.createDescriptorPool(createInfo);
    }
    void Renderer::allocateSets() {
        std::vector<vk::DescriptorSetLayout> layouts(maxFlightCount_, Context::Instance().renderProcess->setLayout);

        vk::DescriptorSetAllocateInfo allocate;
        allocate.setDescriptorPool(descriptorPool_)
                .setDescriptorSetCount(maxFlightCount_)
                .setSetLayouts(layouts);
        sets_ = Context::Instance().device.allocateDescriptorSets(allocate);
    }
    void Renderer::updateSets() {
        for (int i = 0; i < sets_.size(); i++) {
            auto& set = sets_[i];
            vk::DescriptorBufferInfo bufferInfo;
            bufferInfo.setBuffer(deviceUniformBuffer_[i]->buffer);
            bufferInfo.setOffset(0);
            bufferInfo.setRange(deviceUniformBuffer_[i]->size);

            vk::WriteDescriptorSet writer;
            writer.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            writer.setBufferInfo(bufferInfo);
            writer.setDstBinding(0);
            writer.setDstSet(set);
            writer.setDstArrayElement(0);
            writer.setDescriptorCount(1);
            Context::Instance().device.updateDescriptorSets(writer, {});
        }
    }

    void Renderer::copyBuffer(vk::Buffer& src, vk::Buffer& dst, size_t size, size_t srcOffset, size_t dstOffset) {
        auto cmdBuf = Context::Instance().commandManager->CreateOneCommandBuffer();

        vk::CommandBufferBeginInfo begin;
        begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(begin); {
            vk::BufferCopy region;
            region.setSize(size)
                .setSrcOffset(srcOffset)
                .setDstOffset(dstOffset);
            cmdBuf.copyBuffer(src, dst, region);
        } cmdBuf.end();

        vk::SubmitInfo submit;
        submit.setCommandBuffers(cmdBuf);
        Context::Instance().graphicsQueue.submit(submit);

        Context::Instance().device.waitIdle();

        Context::Instance().commandManager->FreeCmd(cmdBuf);
    }
}