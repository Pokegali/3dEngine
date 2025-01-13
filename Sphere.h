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
		double distance;
		bool result = false;
	};

	Sphere(const Vector& center, double radius, const Vector& albedo);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;

	Vector center;
	double radius;
	Vector albedo;
};



#endif //SPHERE_H
