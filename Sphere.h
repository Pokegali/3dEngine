//
// Created by remi on 10/01/25.
//

#ifndef SPHERE_H
#define SPHERE_H

#include "Object.h"
#include "Ray.h"
#include "Vector.h"


class Sphere: public Object {
public:
	Sphere(const Vector& center, double radius, const Vector& albedo);
	Sphere(const Vector& center, double radius, const AlbedoFunction& albedo);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const override;

	Sphere& mirror();
	Sphere& transparent(double opticalIndex);
	Sphere& light(double power);

	Vector center;
	double radius;
};



#endif //SPHERE_H
