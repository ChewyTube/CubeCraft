#pragma once
#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class RenderProcess {
	public:
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;
		vk::RenderPass renderPass;

		void InitLayout();
		void InitRenderPass();
		void InitPipeline();

		void DestroyPipeline();

		VkShaderModule createShaderModule(const std::vector<char>& code);

	private:

	};
}