#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class Shader {
	public:
		Shader(vk::Device device, const std::vector<char> vertSource, const std::vector<char> fragSource);
		~Shader();

		vk::ShaderModule GetVertexModule() const { return vertexModule; }
		vk::ShaderModule GetFragModule() const { return fragmentModule; }

		const std::vector<vk::DescriptorSetLayout>& GetDescriptorSetLayouts() const { return layouts_; }
		vk::PushConstantRange GetPushConstantRange() const;
	private:
		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;
		std::vector<vk::DescriptorSetLayout> layouts_;

		void initDescriptorSetLayouts();
	};

}
