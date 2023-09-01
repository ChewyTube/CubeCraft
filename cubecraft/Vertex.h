#pragma once

#include <vulkan/vulkan.hpp>

namespace cubecraft {
	struct Vertex
	{
		float x, y, z;

		static vk::VertexInputAttributeDescription getAttribute() {
			vk::VertexInputAttributeDescription attr;

			attr.setBinding(0)
				.setFormat(vk::Format::eR32G32B32Sfloat)
				.setLocation(0)
				.setOffset(0);

			return attr;
		}
		static vk::VertexInputBindingDescription getBinding() {
			vk::VertexInputBindingDescription binding;

			binding.setBinding(0);
			binding.setInputRate(vk::VertexInputRate::eVertex);
			binding.setStride(sizeof(Vertex));


			return binding;
		}
	};
}