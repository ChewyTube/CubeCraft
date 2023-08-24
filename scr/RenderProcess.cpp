#include "../cubecraft/RenderProcess.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
    VkShaderModule RenderProcess::createShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(Context::GetInstance().device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

	void RenderProcess::InitPipeline() {
		vk::GraphicsPipelineCreateInfo createInfo;

		//1.顶点输入
		vk::PipelineVertexInputStateCreateInfo inputState;
		createInfo.setPVertexInputState(&inputState);

		//2.图元组合
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.setPrimitiveRestartEnable(false);
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
		createInfo.setPInputAssemblyState(&inputAssembly);

		//3.着色器
		vk::PipelineShaderStageCreateInfo vertexShaderStage;
		vertexShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);
		vertexShaderStage.setModule(Context::GetInstance().vertexModule);
		vertexShaderStage.setPName("main");
		vk::PipelineShaderStageCreateInfo fragmentShaderStage;
		fragmentShaderStage.setStage(vk::ShaderStageFlagBits::eFragment);
		fragmentShaderStage.setModule(Context::GetInstance().fragmentModule);
		fragmentShaderStage.setPName("main");
		auto stages = { vertexShaderStage, fragmentShaderStage };
		createInfo.setStages(stages);

		//4.视口变换
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, WIDTH, HEIGHT, 0, 1);
		vk::Rect2D rect({ 0, 0 }, { WIDTH, HEIGHT });
		viewportState.setViewports(viewport);
		viewportState.setScissors(rect);
		createInfo.setPViewportState(&viewportState);

		//5.光栅化
		vk::PipelineRasterizationStateCreateInfo rastStage;
		rastStage.setRasterizerDiscardEnable(false);
		rastStage.setCullMode(vk::CullModeFlagBits::eBack);
		rastStage.setFrontFace(vk::FrontFace::eCounterClockwise);
		rastStage.setPolygonMode(vk::PolygonMode::eFill);
		rastStage.setLineWidth(1);
		createInfo.setPRasterizationState(&rastStage);

		//6.采样
		vk::PipelineMultisampleStateCreateInfo multState;
		multState.setSampleShadingEnable(false);
		multState.setRasterizationSamples(vk::SampleCountFlagBits::e1);
		createInfo.setPMultisampleState(&multState);

		//7.模板测试 深度测试
		;

		//8.混合
		vk::PipelineColorBlendStateCreateInfo blendState;
		vk::PipelineColorBlendAttachmentState attachState;
		attachState.setBlendEnable(false);
		attachState.setColorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA);
		blendState.setLogicOpEnable(false);
		blendState.setLogicOp(vk::LogicOp::eCopy);
		blendState.setAttachmentCount(1);
		blendState.setAttachments(attachState);
		createInfo.setPColorBlendState(&blendState);

		//9.渲染流程
		createInfo.setRenderPass(renderPass);
		createInfo.setLayout(layout);

		//std::cout << Context::GetInstance().device << std::endl;
		auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr, createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("创建渲染管线失败");
        }
		std::cout << "成功创建渲染管线\n";
		pipeline = result.value;
	}
	void RenderProcess::InitLayout() {
		vk::PipelineLayoutCreateInfo createInfo;
		layout = Context::GetInstance().device.createPipelineLayout(createInfo);
	}
	void RenderProcess::InitRenderPass() {
		vk::RenderPassCreateInfo createInfo;

		vk::AttachmentDescription attachDescr;
		vk::SubpassDescription subpass;
		vk::AttachmentReference reference;
		vk::SubpassDependency dependency;

		attachDescr.setFormat(Context::GetInstance().swapChain->swapchainInfo.imageFormat.format);
		attachDescr.setInitialLayout(vk::ImageLayout::eUndefined);
		attachDescr.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachDescr.setLoadOp(vk::AttachmentLoadOp::eClear);
		attachDescr.setStoreOp(vk::AttachmentStoreOp::eStore);
		attachDescr.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
		attachDescr.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
		attachDescr.setSamples(vk::SampleCountFlagBits::e1);

		reference.setLayout(vk::ImageLayout::eColorAttachmentOptimal);
		reference.setAttachment(0);

		subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
		subpass.setColorAttachments(reference);

		dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
		dependency.setDstSubpass(0);
		dependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
		dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
		dependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

		createInfo.setDependencies(dependency);
		createInfo.setSubpasses(subpass);
		createInfo.setAttachments(attachDescr);

		renderPass = Context::GetInstance().device.createRenderPass(createInfo);
	}
	
	void RenderProcess::DestroyPipeline() {
		auto& device = Context::GetInstance().device;

		device.destroyRenderPass(renderPass);
		device.destroyPipelineLayout(layout);
		device.destroyPipeline(pipeline);
	}
}