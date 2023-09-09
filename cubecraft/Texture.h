#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffer.h"

namespace cubecraft {
    class Texture final {
    public:
        Texture(std::string_view filename);
        ~Texture();

        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;

    private:
        void createImage(uint32_t w, uint32_t h);
        void createImageView();
        void allocMemory();
        //uint32_t queryImageMemoryIndex();
        void transitionImageLayoutFromUndefine_To_Dst();
        void transitionImageLayoutFromDst_To_Optimal();
        void transformData_To_Image(Buffer&, uint32_t w, uint32_t h);
    };
}