#include <filesystem>
#include <fstream>
#include <iostream>

#include "streebog.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cerr << "Usage: " << argv[0] << " [path]" << std::endl;
		return 1;
	}

	std::string path(argv[1]);

	Streebog hash;
	hash.init();

	if (fs::is_regular_file(path)) {
		// Compute hash for the file
		std::ifstream file(path, std::ios::binary);
		if (file.is_open()) {
			hash.update(file);
			file.close();
		}
	} else if (fs::is_directory(path)) {
		// Compute hash for the directory
		for (auto &entry : fs::recursive_directory_iterator(path)) {
			if (fs::is_regular_file(entry.path())) {
				std::ifstream file(entry.path(), std::ios::binary);
				
				if (file.is_open()) {
					hash.update(file);
					file.close();
				}
			}
		}
	} else {
		std::cerr << "Error: Invalid file/directory path" << std::endl;
		return 1;
	}

	std::cout << bytesToHexString(hash.digest()) << "  " << path << std::endl;

	return 0;
}
