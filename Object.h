//
// Created by remi on 14/02/25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include "Ray.h"


class Object {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		double distance = 0;
		Vector albedo;
		bool result = false;
	};

	[[nodiscard]] virtual IntersectResult intersect(const Ray& ray) const = 0;

	explicit Object(const Vector& albedo);
	virtual ~Object() = default;

	bool isLight = false;
	Vector albedo;
	bool mirrors = false;
	bool isTransparent = false;
	double opticalIndex = 1;
	double lightPower = 0;
};



#endif //OBJECT_H
