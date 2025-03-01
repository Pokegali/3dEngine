//
// Created by remi on 01/03/25.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
	int height = -1;
	int width = -1;
	double alpha = -1;
	int raysPerPixel = -1;
	int maxBounce = -1;
	double focusDistance = -1;
};

void readConfig(const std::string& file, Config& config);

#endif