#include "../cubecraft/Context.h"

#include <iostream>

namespace cubecraft{
	void Context::getLayers() {
		auto enableLayers = vk::enumerateInstanceLayerProperties();
		std::cout << "¿ÉÓÃ²ã£º\n";
		for (auto layer : enableLayers) {
			std::cout << "\t" << layer.layerName << std::endl;
		}
	}
}