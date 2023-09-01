#include "../cubecraft/RenderProcess.h"
#include "../cubecraft/Context.h"
#include "../cubecraft/Uniform.h"

namespace cubecraft {
	/*
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
	*/
   

	vk::Pipeline RenderProcess::createGraphicsPipeline(const Shader& shader, vk::PrimitiveTopology topology) {
		
		vk::GraphicsPipelineCreateInfo createInfo;

		//1.顶点输入
		vk::PipelineVertexInputStateCreateInfo inputState;
		auto attr = Vertex::getAttribute();
		auto bind = Vertex::getBinding();
		inputState.setVertexAttributeDescriptions(attr);
		inputState.setVertexBindingDescriptions(bind);
		createInfo.setPVertexInputState(&inputState);

		//2.图元组合
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.setPrimitiveRestartEnable(false);
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
		createInfo.setPInputAssemblyState(&inputAssembly);

		//3.着色器
		std::array<vk::PipelineShaderStageCreateInfo, 2> stageCreateInfos;
		stageCreateInfos[0].setModule(shader.GetVertexModule())
			.setPName("main")
			.setStage(vk::ShaderStageFlagBits::eVertex);
		stageCreateInfos[1].setModule(shader.GetFragModule())
			.setPName("main")
			.setStage(vk::ShaderStageFlagBits::eFragment);
		createInfo.setStages(stageCreateInfos);

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
		rastStage.setFrontFace(vk::FrontFace::eClockwise);
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
		auto result = Context::Instance().device.createGraphicsPipeline(nullptr, createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("创建渲染管线失败");
        }

		std::cout << "成功创建渲染管线：" << result.value << std::endl;
		return result.value;
	}
	vk::PipelineLayout RenderProcess::createLayout() {
		vk::PipelineLayoutCreateInfo createInfo;
		createInfo.setSetLayouts(setLayout);

		return Context::Instance().device.createPipelineLayout(createInfo);
	}
	vk::RenderPass RenderProcess::createRenderPass() {
		vk::RenderPassCreateInfo createInfo;

		vk::AttachmentDescription attachDescr;
		vk::SubpassDescription subpass;
		vk::AttachmentReference reference;
		vk::SubpassDependency dependency;

		attachDescr.setFormat(Context::Instance().swapChain->swapchainInfo.imageFormat.format);
		attachDescr.setInitialLayout(vk::ImageLayout::eUndefined);
		attachDescr.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
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

		return Context::Instance().device.createRenderPass(createInfo);
	}
	vk::DescriptorSetLayout RenderProcess::createSetLayout() {
		vk::DescriptorSetLayoutCreateInfo createInfo;
		auto binding = Uniform::GetBinding();
		createInfo.setBindings(binding);

		return Context::Instance().device.createDescriptorSetLayout(createInfo);
	}
	
	void RenderProcess::CreateGraphicsPipeline(const Shader& shader) {
		graphicsPipelineWithTriangleTopology = createGraphicsPipeline(shader, vk::PrimitiveTopology::eTriangleList);
	}

	void RenderProcess::CreateRenderPass() {
		renderPass = createRenderPass();
	}

	RenderProcess::RenderProcess() {
		setLayout = createSetLayout();
		layout = createLayout();
		CreateRenderPass();
		graphicsPipelineWithTriangleTopology = nullptr;
	}
	RenderProcess::~RenderProcess() {
		auto& device = Context::Instance().device;

		//device.destroyPipelineCache(pipelineCache_);
		device.destroyDescriptorSetLayout(setLayout);
		device.destroyRenderPass(renderPass);
		device.destroyPipelineLayout(layout);
		device.destroyPipeline(graphicsPipelineWithTriangleTopology);
		//device.destroyPipeline(graphicsPipelineWithLineTopology);
	}
}