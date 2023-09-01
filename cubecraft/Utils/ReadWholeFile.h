#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

namespace cubecraft {
	std::vector<char> ReadWholeFile(const std::string& filename);
    std::vector<char> readFile(const std::string& filename);

}