#include "../cubecraft/Context.h"

namespace cubecraft {
	void Context::createSwapChain() {
		querySwapChainInfo();

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setSurface(surface)
			.setImageColorSpace(swapChainInfo.imageFormat.colorSpace)
			.setImageFormat(swapChainInfo.imageFormat.format)
			.setImageExtent(swapChainInfo.imageExtent)
			.setMinImageCount(swapChainInfo.imageCount)
			.setPresentMode(swapChainInfo.present);

		const uint32_t presentQueue = queueFamilyIndecis.presentQueue.value();
		if (queueFamilyIndecis.graphicsQueue.value() == queueFamilyIndecis.presentQueue.value()) {
			createInfo.setQueueFamilyIndices(queueFamilyIndecis.graphicsQueue.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else {
			std::array indices = { queueFamilyIndecis.graphicsQueue.value(), queueFamilyIndecis.presentQueue.value() };
			createInfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);
		}

		swapchain = device.createSwapchainKHR(createInfo);
	}
	void Context::querySwapChainInfo() {
		uint32_t w = WIDTH;
		uint32_t h = HEIGHT;

		auto formats = phyDevice.getSurfaceFormatsKHR(surface);
		swapChainInfo.imageFormat = formats[0];
		for (const auto& format : formats) {
			if (format.format == vk::Format::eR8G8B8A8Srgb &&
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				swapChainInfo.imageFormat = format;
				break;
			}
		}

		auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		swapChainInfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);

		swapChainInfo.imageExtent.width = std::clamp(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		swapChainInfo.imageExtent.height = std::clamp(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		swapChainInfo.transform = capabilities.currentTransform;

		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		swapChainInfo.present = vk::PresentModeKHR::eFifo;
		for (const auto& present : presents) {
			if (present == vk::PresentModeKHR::eMailbox) {
				swapChainInfo.present = present;
				break;
			}
		}
	}

	void Context::getImages() {
		images = device.getSwapchainImagesKHR(swapchain);
	}
	void Context::createImageViews() {
		imageViews.resize(images.size());

		for (int i = 0; i < images.size(); i++) {
			vk::ImageViewCreateInfo createInfo;
			vk::ComponentMapping mapping;
			vk::ImageSubresourceRange range;

			mapping.setA(vk::ComponentSwizzle::eA);
			mapping.setR(vk::ComponentSwizzle::eR);
			mapping.setG(vk::ComponentSwizzle::eG);
			mapping.setB(vk::ComponentSwizzle::eB);

			range.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				.setAspectMask(vk::ImageAspectFlagBits::eColor);

			createInfo.setImage(images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setComponents(mapping)
				.setFormat(swapChainInfo.imageFormat.format)
				.setSubresourceRange(range);

			imageViews[i] = device.createImageView(createInfo);
		}
	}
}