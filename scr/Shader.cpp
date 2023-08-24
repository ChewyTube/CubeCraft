#include "../cubecraft/Shader.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
	Shader::Shader(vk::Device device, const std::string& vertSource, const std::string& fragSource) {
		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(vertSource.size());
		createInfo.setPCode((uint32_t*)vertSource.data());
		vertexModule = device.createShaderModule(createInfo);
		
		createInfo.setCodeSize(fragSource.size());
		createInfo.setPCode((uint32_t*)fragSource.data());
		fragmentModule = device.createShaderModule(createInfo);
	}
	
	void Shader::destroyShaderModules(vk::Device device) {
		device.destroyShaderModule(vertexModule);
		device.destroyShaderModule(fragmentModule);
	}
	
	void Context::createShader(const std::string& vertSource, const std::string& fragSource) {
		vk::ShaderModuleCreateInfo createInfo;
		createInfo.setCodeSize(vertSource.size());
		createInfo.setPCode((uint32_t*)vertSource.data());
		vertexModule = device.createShaderModule(createInfo);

		createInfo.setCodeSize(fragSource.size());
		createInfo.setPCode((uint32_t*)fragSource.data());
		fragmentModule = device.createShaderModule(createInfo);
	}
	/*
	void Context::destroyShader() {
		device.destroyShaderModule(vertexModule);
		device.destroyShaderModule(fragmentModule);
	}

	
	Shader::~Shader() {
		auto device = Context::GetInstance().device;
		device.destroyShaderModule(vertexModule);
		device.destroyShaderModule(fragmentModule);
	}
	*/
	
}