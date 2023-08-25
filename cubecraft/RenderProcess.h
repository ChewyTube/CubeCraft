#pragma once
#include <vulkan/vulkan.hpp>
#include "Shader.h"

namespace cubecraft {
	class RenderProcess final{
	public:
		vk::Pipeline graphicsPipelineWithTriangleTopology = nullptr;
		vk::RenderPass renderPass = nullptr;
		vk::PipelineLayout layout = nullptr;

		
		void CreateGraphicsPipeline(const Shader& shader);
		void CreateRenderPass();

		//VkShaderModule createShaderModule(const std::vector<char>& code);

		RenderProcess();
		~RenderProcess();
	private:
		vk::PipelineLayout createLayout();
		vk::Pipeline createGraphicsPipeline(const Shader& shader, vk::PrimitiveTopology);
		vk::RenderPass createRenderPass();
		//vk::PipelineCache createPipelineCache();
	};
}