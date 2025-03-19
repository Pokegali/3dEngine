//
// Created by remi on 14/02/25.
//

#ifndef OBJECT_H
#define OBJECT_H

#include <functional>

#include "Ray.h"

class Object {
public:
	typedef std::function<Vector (const Vector& impact)> AlbedoFunction;

	struct IntersectResult {
		Vector impact;
		Vector normal;
		double distance = 0;
		Vector albedo;
		bool result = false;
	};

	[[nodiscard]] virtual IntersectResult intersect(const Ray& ray) const = 0;

	explicit Object(const Vector& albedo);
	explicit Object(AlbedoFunction albedo);
	virtual ~Object() = default;

	bool isLight = false;
	Vector albedo;
	bool hasVariableAlbedo = false;
	AlbedoFunction albedoFunction;
	bool mirrors = false;
	bool isTransparent = false;
	double opticalIndex = 1;
	double lightPower = 0;
};

namespace AlbedoFunctions {
	Object::AlbedoFunction checkerboard(uint32_t axis, double size, Vector albedo1, Vector albedo2);
}


#endif //OBJECT_H
