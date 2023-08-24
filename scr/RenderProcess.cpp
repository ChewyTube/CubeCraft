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

		//1.��������
		vk::PipelineVertexInputStateCreateInfo inputState;
		createInfo.setPVertexInputState(&inputState);

		//2.ͼԪ���
		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.setPrimitiveRestartEnable(false);
		inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
		createInfo.setPInputAssemblyState(&inputAssembly);

		//3.��ɫ��
		vk::PipelineShaderStageCreateInfo vertexShaderStage;
		vertexShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);
		vertexShaderStage.setModule(Context::GetInstance().vertexModule);
		vertexShaderStage.setPName("main");
		vk::PipelineShaderStageCreateInfo fragmentShaderStage;
		fragmentShaderStage.setStage(vk::ShaderStageFlagBits::eVertex);
		fragmentShaderStage.setModule(Context::GetInstance().fragmentModule);
		fragmentShaderStage.setPName("main");
		auto stages = { vertexShaderStage, fragmentShaderStage };
		createInfo.setStages(stages);

		//4.�ӿڱ任
		vk::PipelineViewportStateCreateInfo viewportState;
		vk::Viewport viewport(0, 0, WIDTH, HEIGHT, 0, 1);
		vk::Rect2D rect({ 0, 0 }, { WIDTH, HEIGHT });
		viewportState.setViewports(viewport);
		viewportState.setScissors(rect);
		createInfo.setPViewportState(&viewportState);

		//5.��դ��
		vk::PipelineRasterizationStateCreateInfo rastStage;
		rastStage.setRasterizerDiscardEnable(false);
		rastStage.setCullMode(vk::CullModeFlagBits::eBack);
		rastStage.setFrontFace(vk::FrontFace::eCounterClockwise);
		rastStage.setPolygonMode(vk::PolygonMode::eFill);
		rastStage.setLineWidth(1);
		createInfo.setPRasterizationState(&rastStage);

		//6.����
		vk::PipelineMultisampleStateCreateInfo multState;
		multState.setSampleShadingEnable(false);
		multState.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		//7.ģ����� ��Ȳ���
		;

		//8.���
		vk::PipelineColorBlendStateCreateInfo blendState;
		vk::PipelineColorBlendAttachmentState attachState;
		attachState.setBlendEnable(false);
		attachState.setColorWriteMask(
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA);
		blendState.setLogicOpEnable(false);
		blendState.setAttachments(attachState);
		auto result = Context::GetInstance().device.createGraphicsPipeline(nullptr,createInfo);
		if (result.result != vk::Result::eSuccess) {
			throw std::runtime_error("������Ⱦ����ʧ��");
        }
		std::cout << "�ɹ�������Ⱦ����\n";
		pipeline = result.value;
	}

	RenderProcess* RenderProcess::GetRenderProcess() {
		return this;
	}
	void RenderProcess::DestroyPipeline() {
		Context::GetInstance().device.destroyPipeline(pipeline);
	}
}