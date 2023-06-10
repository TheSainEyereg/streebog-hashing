#pragma once

#include <sstream>
#include <string>
#include <vector>

// https://stackoverflow.com/a/14051107
static std::string bytesToHexString(const std::vector<uint8_t> &bytes) {
	std::stringstream ss;
	ss << std::setfill('0') << std::hex;

	for (int i = 0; i < bytes.size(); i++) {
		ss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
	}

	return ss.str();
}