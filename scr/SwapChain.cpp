#include "../cubecraft/Context.h"
#include "../cubecraft/SwapChain.h"

namespace cubecraft {
	SwapChain::SwapChain() {
		queryswapchainInfo();
		auto& queueFamilyIndecis = Context::Instance().queueFamilyIndecis;

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setSurface(Context::Instance().surface)
			.setImageColorSpace(swapchainInfo.imageFormat.colorSpace)
			.setImageFormat(swapchainInfo.imageFormat.format)
			.setImageExtent(swapchainInfo.imageExtent)
			.setMinImageCount(swapchainInfo.imageCount)
			.setPresentMode(swapchainInfo.present);

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

		swapchain = Context::Instance().device.createSwapchainKHR(createInfo);

		getImages();
		createImageViews();
	}
	void SwapChain::DestroySwapChain(){
		for (auto& view : imageViews) {
			Context::Instance().device.destroyImageView(view);
		}
		for (auto& framebuffer : framebuffers) {
			Context::Instance().device.destroyFramebuffer(framebuffer);
		}
		Context::Instance().device.destroySwapchainKHR(swapchain);
		Context::Instance().instance.destroySurfaceKHR(Context::Instance().surface);
	}
	void SwapChain::queryswapchainInfo() {
		uint32_t w = WIDTH;
		uint32_t h = HEIGHT;

		auto& phyDevice = Context::Instance().phyDevice;
		auto& surface = Context::Instance().surface;

		auto formats = phyDevice.getSurfaceFormatsKHR(surface);
		swapchainInfo.imageFormat = formats[0];
		for (const auto& format : formats) {
			if (format.format == vk::Format::eR8G8B8A8Srgb &&
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				swapchainInfo.imageFormat = format;
				break;
			}
		}

		auto capabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		swapchainInfo.imageCount = std::clamp<uint32_t>(2, capabilities.minImageCount, capabilities.maxImageCount);

		swapchainInfo.imageExtent.width = std::clamp(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		swapchainInfo.imageExtent.height = std::clamp(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		swapchainInfo.transform = capabilities.currentTransform;

		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		swapchainInfo.present = vk::PresentModeKHR::eFifo;
		for (const auto& present : presents) {
			if (present == vk::PresentModeKHR::eMailbox) {
				swapchainInfo.present = present;
				break;
			}
		}
	}
	void SwapChain::getImages() {
		images = Context::Instance().device.getSwapchainImagesKHR(swapchain);
	}
	void SwapChain::createImageViews() {
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
				.setFormat(swapchainInfo.imageFormat.format)
				.setSubresourceRange(range);

			imageViews[i] = Context::Instance().device.createImageView(createInfo);
		}
	}
	void SwapChain::createFrameBuffers() {
		framebuffers.resize(images.size());
		for (int i = 0; i < framebuffers.size(); i++) {
			vk::FramebufferCreateInfo createInfo;
			createInfo.setAttachments(imageViews[i]);
			createInfo.setWidth(WIDTH);
			createInfo.setHeight(HEIGHT);
			createInfo.setRenderPass(Context::Instance().renderProcess->renderPass);
			createInfo.setLayers(1);

			framebuffers[i] = Context::Instance().device.createFramebuffer(createInfo);
		}
	}
}