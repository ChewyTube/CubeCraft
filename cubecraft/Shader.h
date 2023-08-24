#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class Shader {
	public:
		Shader(vk::Device device, const std::string& vertSource, const std::string& fragSource);
		//~Shader();
		void destroyShaderModules(vk::Device device);

		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;
	private:
	};

}
