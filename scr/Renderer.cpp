#include "../cubecraft/Renderer.h"
#include "../cubecraft/Context.h"

#include <chrono>

namespace cubecraft {
    const Uniform uniform{ Color{1, 0, 0} };

	Renderer::Renderer(int maxFlightCount) : maxFlightCount_(maxFlightCount), curFrame_(0) {
        createFence();
        createSems();
        createCmdBuffers();
        createBuffers();
        createUniformBuffers();
        bufferData();
        createTexture();
        createSampler();
        createDescriptorPool();
        allocDescriptorSets();
        updateDescriptorSets();
	}
	Renderer::~Renderer() {
		auto& device = Context::Instance().device;
        device.destroySampler(sampler);
        texture.reset();
        device.destroyDescriptorPool(descriptorPool_);
        verticesBuffer_.reset();
        indicesBuffer_.reset();
        uniformBuffers_.clear();

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

        bufferMVPData();

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
        
        cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, renderProcess->graphicsPipelineWithTriangleTopology);
        vk::DeviceSize offset = 0;
        cmd.bindVertexBuffers(0, verticesBuffer_->buffer, offset);
        cmd.bindIndexBuffer(indicesBuffer_->buffer, 0, vk::IndexType::eUint32);
        cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, Context::Instance().renderProcess->layout, 0, sets_[curFrame_], {});

        cmd.drawIndexed(6, 1, 0, 0, 0);
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

    void Renderer::createSampler() {
        vk::SamplerCreateInfo createInfo;
        createInfo.setMagFilter(vk::Filter::eNearest)
            .setMinFilter(vk::Filter::eNearest)
            .setAddressModeU(vk::SamplerAddressMode::eRepeat)
            .setAddressModeV(vk::SamplerAddressMode::eRepeat)
            .setAddressModeW(vk::SamplerAddressMode::eRepeat)
            .setAnisotropyEnable(false)
            .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
            .setUnnormalizedCoordinates(false)
            .setCompareEnable(false)
            .setMipmapMode(vk::SamplerMipmapMode::eNearest);
        sampler = Context::Instance().device.createSampler(createInfo);
    }

    void Renderer::createBuffers(){
        auto& device = Context::Instance().device;

        verticesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eVertexBuffer,
            sizeof(Vertex) * 4,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));

        indicesBuffer_.reset(new Buffer(vk::BufferUsageFlagBits::eIndexBuffer,
            sizeof(uint32_t) * 6,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
    }
    void Renderer::bufferData() {
        bufferVertexData();
        bufferIndicesData();
    }
    void Renderer::createUniformBuffers() {
        uniformBuffers_.resize(maxFlightCount_);
        
        size_t size = sizeof(MVP);
        for (auto& buffer : uniformBuffers_) {
            buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferSrc,
                size,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent));
        }
        deviceUniformBuffers_.resize(maxFlightCount_);
        for (auto& buffer : deviceUniformBuffers_) {
            buffer.reset(new Buffer(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
                size,
                vk::MemoryPropertyFlagBits::eDeviceLocal));
        }
    }
    void Renderer::bufferVertexData(){
        const Vertex vertices[] = {
            {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
            {{0.5f,  -0.5f, 0.0f}, {1.0f, 0.0f}},
            {{0.5f,  0.5f,  0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f,  0.0f}, {0.0f, 1.0f}},
        };
        auto& device = Context::Instance().device;
        memcpy(verticesBuffer_->map, vertices, sizeof(vertices));
    }
    void Renderer::bufferIndicesData() {
        std::uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
        };
        auto& device = Context::Instance().device;
        memcpy(indicesBuffer_->map, indices, sizeof(indices));
    }
    
    void Renderer::bufferMVPData() {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        MVP mvp;
        mvp.model = glm::mat4(1.0f);
        mvp.view  = Context::Instance().camera.GetViewMatrix();
        mvp.proj  = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 256.0f);
        mvp.proj[1][1] *= -1;
        
        auto& device = Context::Instance().device;
        for (int i = 0; i < uniformBuffers_.size(); i++) {
            auto& buffer = uniformBuffers_[i];
            memcpy(buffer->map, (void*)&mvp, sizeof(mvp));
            transformBuffer_To_Device(*buffer, *deviceUniformBuffers_[i], 0, 0, buffer->size);
        }
    }

    void Renderer::createTexture() {
        texture.reset(new Texture("D:/Vulkan/Program/CubeCraft/resources/dirt.png"));
    }

    void Renderer::createDescriptorPool() {
        vk::DescriptorPoolCreateInfo createInfo; 

        std::vector<vk::DescriptorPoolSize> sizes(2);
        sizes[0].setDescriptorCount(maxFlightCount_*3)
            .setType(vk::DescriptorType::eUniformBuffer);
        sizes[1].setDescriptorCount(maxFlightCount_)
            .setType(vk::DescriptorType::eCombinedImageSampler);

        createInfo.setMaxSets(maxFlightCount_);
        createInfo.setPoolSizes(sizes);

        descriptorPool_ = Context::Instance().device.createDescriptorPool(createInfo);
    }
    std::vector<vk::DescriptorSet> Renderer::allocDescriptorSet(int maxFlight) {
        std::vector layouts(maxFlight, Context::Instance().shader->GetDescriptorSetLayouts()[0]);
        vk::DescriptorSetAllocateInfo allocInfo;
        allocInfo.setDescriptorPool(descriptorPool_);
        allocInfo.setSetLayouts(layouts);
        return Context::Instance().device.allocateDescriptorSets(allocInfo);
    }
    void Renderer::allocDescriptorSets() {
        sets_ = allocDescriptorSet(maxFlightCount_);
    }

    void Renderer::updateDescriptorSets() {
        for (int i = 0; i < sets_.size(); i++) {
            // bind MVP buffer
            vk::DescriptorBufferInfo mvpBufferInfo;
            mvpBufferInfo.setBuffer(deviceUniformBuffers_[i]->buffer)
                .setOffset(0)
                .setRange(sizeof(MVP));

            std::vector<vk::WriteDescriptorSet> writeInfos(2);
            writeInfos[0].setBufferInfo(mvpBufferInfo)
                .setDstBinding(0)
                .setDescriptorType(vk::DescriptorType::eUniformBuffer)
                .setDescriptorCount(1)
                .setDstArrayElement(0)
                .setDstSet(sets_[i]);

            // bind image
            vk::DescriptorImageInfo imageInfo;
            imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
                .setImageView(texture->view)
                .setSampler(sampler);

            writeInfos[1].setImageInfo(imageInfo)
                .setDstBinding(1)
                .setDescriptorCount(1)
                .setDstArrayElement(0)
                .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
                .setDstSet(sets_[i]);

            Context::Instance().device.updateDescriptorSets(writeInfos, {});
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
    void Renderer::transformBuffer_To_Device(Buffer& src, Buffer& dst, size_t srcOffset, size_t dstOffset, size_t size) {
        auto cmdBuf = Context::Instance().commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);
        vk::BufferCopy region;
        region.setSrcOffset(srcOffset)
            .setDstOffset(dstOffset)
            .setSize(size);
        cmdBuf.copyBuffer(src.buffer, dst.buffer, region);
        cmdBuf.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf);
        Context::Instance().graphicsQueue.submit(submitInfo);
        Context::Instance().graphicsQueue.waitIdle();
        Context::Instance().device.waitIdle();
        Context::Instance().commandManager->FreeCmd(cmdBuf);
    }
}