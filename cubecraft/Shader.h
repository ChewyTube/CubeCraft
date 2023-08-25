#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class Shader {
	public:
		Shader(vk::Device device, const std::vector<char> vertSource, const std::vector<char> fragSource);
		~Shader();
		void destroyShaderModules(vk::Device device);

		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;

		vk::ShaderModule GetVertexModule() const { return vertexModule; }
		vk::ShaderModule GetFragModule() const { return fragmentModule; }
	private:
	};

}
