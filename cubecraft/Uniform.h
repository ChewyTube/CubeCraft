#pragma once

#include <vulkan/vulkan.hpp>

namespace cubecraft {
	struct Color final
	{
		float r, g, b;
	};

	class Uniform final {
	public:
		Color color;

		static vk::DescriptorSetLayoutBinding GetBinding() {
			vk::DescriptorSetLayoutBinding binding;
			binding.setBinding(0);
			binding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
			binding.setStageFlags(vk::ShaderStageFlagBits::eFragment);
			binding.setDescriptorCount(1);

			return binding;
		}
	};
}