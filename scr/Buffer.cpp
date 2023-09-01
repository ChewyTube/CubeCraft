#include "../cubecraft/Buffer.h"
#include "../cubecraft/Context.h"

namespace cubecraft {
	Buffer::Buffer(vk::BufferUsageFlags usage, size_t size, vk::MemoryPropertyFlags memProperty) {
		auto& device = Context::Instance().device;

		this->size = size;
		vk::BufferCreateInfo createInfo;
		createInfo.setUsage(usage)
			.setSize(size)
			.setSharingMode(vk::SharingMode::eExclusive);

		buffer = device.createBuffer(createInfo);

		auto requirements = device.getBufferMemoryRequirements(buffer);
		requireSize = requirements.size;
		auto index = queryBufferMemTypeIndex(requirements.memoryTypeBits, memProperty);
		vk::MemoryAllocateInfo allocInfo;
		allocInfo.setMemoryTypeIndex(index)
			.setAllocationSize(requirements.size);
		memory = device.allocateMemory(allocInfo);

		device.bindBufferMemory(buffer, memory, 0);

		if (memProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
			map = device.mapMemory(memory, 0, size);
		}
		else {
			map = nullptr;
		}
	}
	Buffer::~Buffer() {
		auto& device = Context::Instance().device;
		if (map) {
			device.unmapMemory(memory);
		}
		device.freeMemory(memory);
		device.destroyBuffer(buffer);
	}

	void Buffer::createBuffer(size_t size, vk::BufferUsageFlags usage) {
		vk::BufferCreateInfo createInfo;

		createInfo.setSize(size);
		createInfo.setUsage(usage);
		createInfo.setSharingMode(vk::SharingMode::eExclusive);

		buffer = Context::Instance().device.createBuffer(createInfo);
	}
	void Buffer::allocateMemory(MemoryInfo info) {
		vk::MemoryAllocateInfo allocate;
		allocate.setMemoryTypeIndex(info.index)
				.setAllocationSize(info.size);
		memory = Context::Instance().device.allocateMemory(allocate);
	} 
	void Buffer::bindinfMemory_To_Buffer() {
		Context::Instance().device.bindBufferMemory(buffer, memory, 0);
	}

	Buffer::MemoryInfo Buffer::queryMemoryInfo(vk::MemoryPropertyFlags property) {
		MemoryInfo info;
		auto requirements = Context::Instance().device.getBufferMemoryRequirements(buffer);
		info.size = requirements.size;

		auto properties = Context::Instance().phyDevice.getMemoryProperties();
		for (int i = 0; i < properties.memoryTypeCount; i++){
			if ((1 << i) & requirements.memoryTypeBits &&
				properties.memoryTypes[i].propertyFlags & property) {
				info.index = i;
				break;
			}
		}

		return info;
	}
	std::uint32_t Buffer::queryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag) {
		auto property = Context::Instance().phyDevice.getMemoryProperties();

		for (std::uint32_t i = 0; i < property.memoryTypeCount; i++) {
			if ((1 << i) & type &&
				property.memoryTypes[i].propertyFlags & flag) {
				return i;
			}
		}

		return 0;
	}
}