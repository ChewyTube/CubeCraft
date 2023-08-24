#pragma once
#include <vulkan/vulkan.hpp>

namespace cubecraft {
	class RenderProcess {
	public:
		vk::Pipeline pipeline;

		void InitPipeline();
		void DestroyPipeline();
		RenderProcess* GetRenderProcess();
		VkShaderModule createShaderModule(const std::vector<char>& code);
	private:

	};
}