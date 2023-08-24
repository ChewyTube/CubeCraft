#include "../cubecraft/Context.h"
#include "../cubecraft/SwapChain.h"

namespace cubecraft {
	SwapChain::SwapChain() {
		queryswapchainInfo();
		auto& queueFamilyIndecis = Context::GetInstance().queueFamilyIndecis;

		vk::SwapchainCreateInfoKHR createInfo;
		createInfo.setClipped(true)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setSurface(Context::GetInstance().surface)
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

		swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);
	}
	void SwapChain::DestroySwapChain(){
		for (auto& view : imageViews) {
			Context::GetInstance().device.destroyImageView(view);
		}
		for (auto& framebuffer : framebuffers) {
			Context::GetInstance().device.destroyFramebuffer(framebuffer);
		}
		Context::GetInstance().device.destroySwapchainKHR(swapchain);
	}
	void SwapChain::queryswapchainInfo() {
		uint32_t w = WIDTH;
		uint32_t h = HEIGHT;

		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& surface = Context::GetInstance().surface;

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
		images = Context::GetInstance().device.getSwapchainImagesKHR(swapchain);
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

			imageViews[i] = Context::GetInstance().device.createImageView(createInfo);
		}
	}
	void SwapChain::createFrameBuffers() {
		framebuffers.resize(images.size());
		for (int i = 0; i < framebuffers.size(); i++) {
			vk::FramebufferCreateInfo createInfo;
			createInfo.setAttachments(imageViews[i]);
			createInfo.setWidth(WIDTH);
			createInfo.setHeight(HEIGHT);
			createInfo.setRenderPass(Context::GetInstance().renderProcess->renderPass);
			createInfo.setLayers(1);

			framebuffers[i] = Context::GetInstance().device.createFramebuffer(createInfo);
		}
	}
}