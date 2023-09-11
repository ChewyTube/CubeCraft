#pragma once

#include <vulkan/vulkan.hpp>

#include "Buffer.h"
#include "DescriptorSetManager.h"

namespace cubecraft {
    class Texture final {
    public:
        Texture(std::string_view filename);
        ~Texture();

        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;
        DescriptorSetManager::SetInfo set;

    private:
        void createImage(uint32_t w, uint32_t h);
        void createImageView();
        void allocMemory();
        void transitionImageLayoutFromUndefine_To_Dst();
        void transitionImageLayoutFromDst_To_Optimal();
        void transformData_To_Image(Buffer&, uint32_t w, uint32_t h);
        void updateDescriptorSet();
    };

    class TextureManager final {
    public:
        static TextureManager& Instance() {
            if (!instance_) {
                instance_.reset(new TextureManager);
            }
            return *instance_;
        }

        Texture* Load(const std::string& filename);
        void Destroy(Texture*);
        void Clear();

    private:
        static std::unique_ptr<TextureManager> instance_;

        std::vector<std::unique_ptr<Texture>> datas_;
    };
}