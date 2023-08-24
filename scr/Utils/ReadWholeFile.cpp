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

	std::cout << "�ɹ���ȡ�ļ���" << filename << std::endl;
	return content;
}
