#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

namespace cubecraft {
	std::string ReadWholeFile(const std::string& filename);
    std::vector<char> readFile(const std::string& filename);

}