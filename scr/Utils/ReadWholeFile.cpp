#include "../../cubecraft/Utils/ReadWholeFile.h"

std::vector<char> ReadWholeFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		std::cout << "read " << filename << " failed" << std::endl;
		return std::vector<char>{};
	}

	auto size = file.tellg();
	std::vector<char> content(size);
	file.seekg(0);

	file.read(content.data(), content.size());

	return content;
}

std::vector<char> cubecraft::readFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

