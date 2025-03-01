//
// Created by remi on 10/01/25.
//

#include "Config.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// Taken from https://stackoverflow.com/a/46711735
constexpr uint32_t hash(const std::string_view data) noexcept {
	uint32_t hash = 5385;
	for (const auto& e: data) {
		hash = (hash << 5) + hash + e;
	}
	return hash;
}

constexpr uint32_t operator ""_(const char* p, size_t) { return hash(p); }

void readConfig(const std::string& file, Config& config) {
	std::ifstream stream(file);
	std::string line;
	while (std::getline(stream, line)) {
		std::erase_if(line, isspace);
		std::istringstream lineStream(line);
		std::string key;
		std::string value;
		if (!std::getline(lineStream, key, '=') || !std::getline(lineStream, value)) { continue; }
		switch (hash(key)) {
			case "height"_:
				config.height = std::stoi(value);
				break;
			case "width"_:
				config.width = std::stoi(value);
				break;
			case "alpha"_:
				config.alpha = std::stod(value);
				break;
			case "raysPerPixel"_:
				config.raysPerPixel = std::stoi(value);
				break;
			case "maxBounce"_:
				config.maxBounce = std::stoi(value);
				break;
			case "focusDistance"_:
				config.focusDistance = std::stod(value);
				break;
			default:
				std::cerr << "Warning: Unknown config key '" << key << "'\n";
		}
	}
	std::vector<std::pair<std::string, int>> fields {
		{"height", config.height},
		{"width", config.width},
		{"alpha", config.alpha},
		{"raysPerPixel", config.raysPerPixel},
		{"maxBounce", config.maxBounce},
		{"focusDistance", config.focusDistance}
	};
	for (const auto& [name, field]: fields) {
		if (field == -1) {
			std::cerr << "Warning: " << name << " has not been set\n";
		}
	}
}
