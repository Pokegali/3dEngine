//
// Created by remi on 14/02/25.
//

#include "Object.h"

#include <cmath>

Object::Object(const Vector& albedo): albedo(albedo) {}
Object::Object(AlbedoFunction albedo): hasVariableAlbedo(true), albedoFunction(std::move(albedo)) {}

Object::AlbedoFunction AlbedoFunctions::checkerboard(uint32_t axis, double size, Vector albedo1, Vector albedo2) {
	return [axis, size, albedo1, albedo2](const Vector& impact) {
		bool i1 = static_cast<int>(std::floor(impact[(axis + 1) % 3] / size)) & 1;
		bool i2 = static_cast<int>(std::floor(impact[(axis + 2) % 3] / size)) & 1;
		return i1 ^ i2 ? albedo1 : albedo2;
	};
}
