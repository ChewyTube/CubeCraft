#include "../cubecraft/Shader.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
	Shader::Shader(vk::Device device, const std::vector<char> vertSource, const std::vector<char> fragSource) {
		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(vertSource.size());
		createInfo.setPCode((uint32_t*)vertSource.data());
		vertexModule = device.createShaderModule(createInfo);
		
		createInfo.setCodeSize(fragSource.size());
		createInfo.setPCode((uint32_t*)fragSource.data());
		fragmentModule = device.createShaderModule(createInfo);

		initDescriptorSetLayouts();
	}
	Shader::~Shader() {
		auto& device = Context::Instance().device;
		for (auto& layout : layouts_) {
			device.destroyDescriptorSetLayout(layout);
		}
		layouts_.clear();
		device.destroyShaderModule(vertexModule);
		device.destroyShaderModule(fragmentModule);
	}

	void Shader::initDescriptorSetLayouts() {
		vk::DescriptorSetLayoutCreateInfo createInfo;
		std::vector<vk::DescriptorSetLayoutBinding> bindings(2);
		bindings[0].setBinding(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex);
		bindings[1].setBinding(1) 
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setStageFlags(vk::ShaderStageFlagBits::eFragment);

		createInfo.setBindings(bindings);

		layouts_.push_back(Context::Instance().device.createDescriptorSetLayout(createInfo));
	}

	vk::PushConstantRange Shader::GetPushConstantRange() const {
		vk::PushConstantRange range;
		range.setOffset(0)
			.setSize(sizeof(glm::mat4))
			.setStageFlags(vk::ShaderStageFlagBits::eVertex);
		return range;
	}
}