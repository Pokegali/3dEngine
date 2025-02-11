//
// Created by remi on 10/01/25.
//

#ifndef SPHERE_H
#define SPHERE_H
#include "Ray.h"
#include "Vector.h"


class Sphere {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		double distance = 0;
		bool result = false;
	};

	Sphere(const Vector& center, double radius, const Vector& albedo);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;

	Sphere& mirror();
	Sphere& transparent(double opticalIndex);
	Sphere& light(double power);

	Vector center;
	double radius;
	Vector albedo;
	bool mirrors = false;
	bool isTransparent = false;
	bool isLight = false;
	double opticalIndex = 1;
	double lightPower = 0;
};



#endif //SPHERE_H
