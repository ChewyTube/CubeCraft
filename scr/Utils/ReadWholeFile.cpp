#include "ReadWholeFile.h"

std::string cubecraft::ReadWholeFile(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		std::cout << "failed to read file:" << filename << std::endl;
		return std::string{};
	}

	auto size = file.tellg();
	std::string content;
	content.resize(size);
	file.seekg(0);

	file.read(content.data(), content.size());

	std::cout << "成功读取文件：" << filename << std::endl;
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

