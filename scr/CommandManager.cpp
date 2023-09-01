#include "../cubecraft/CommandManager.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
    CommandManager::CommandManager() {
        //std::cout << "114514\n";
        pool_ = createCommandPool();
    }

    CommandManager::~CommandManager() {
        auto& ctx = Context::Instance();
        ctx.device.destroyCommandPool(pool_);
    }

    void CommandManager::ResetCmds() {
        Context::Instance().device.resetCommandPool(pool_);
    }

    vk::CommandPool CommandManager::createCommandPool() {
        auto& ctx = Context::Instance();

        vk::CommandPoolCreateInfo createInfo;

        createInfo.setQueueFamilyIndex(ctx.queueFamilyIndecis.graphicsQueue.value())
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

        return ctx.device.createCommandPool(createInfo);
    }

    std::vector<vk::CommandBuffer> CommandManager::CreateCommandBuffers(std::uint32_t count) {
        auto& ctx = Context::Instance();

        vk::CommandBufferAllocateInfo allocInfo;
        allocInfo.setCommandPool(pool_)
            .setCommandBufferCount(1)
            .setLevel(vk::CommandBufferLevel::ePrimary);

        return ctx.device.allocateCommandBuffers(allocInfo);
    }

    vk::CommandBuffer CommandManager::CreateOneCommandBuffer() {
        return CreateCommandBuffers(1)[0];
    }

    void CommandManager::FreeCmd(const vk::CommandBuffer& cmdBuf) {
        Context::Instance().device.freeCommandBuffers(pool_, cmdBuf);
    }

    void CommandManager::ExecuteCmd(vk::Queue queue, RecordCmdFunc func) {
        auto cmdBuf = CreateOneCommandBuffer();

        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);
        if (func) func(cmdBuf);
        cmdBuf.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(cmdBuf);
        queue.submit(submitInfo);
        queue.waitIdle();
        Context::Instance().device.waitIdle();
        FreeCmd(cmdBuf);
    }
}