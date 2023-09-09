#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

namespace cubecraft {
    struct Vertex {
        glm::vec3 pos;
        glm::vec2 texCoord;

        static vk::VertexInputBindingDescription getBindingDescription() {
            vk::VertexInputBindingDescription bindingDescription;
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = vk::VertexInputRate::eVertex;

            return bindingDescription;
        }

        static std::vector<vk::VertexInputAttributeDescription> getAttributeDescriptions() {
            std::vector<vk::VertexInputAttributeDescription> attributeDescriptions(2);

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
            attributeDescriptions[0].offset = 0;

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = vk::Format::eR32G32Sfloat;
            attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

            return attributeDescriptions;
        }
    };
}