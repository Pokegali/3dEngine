//
// Created by remi on 10/01/25.
//

#include "Sphere.h"

#include <cmath>

#include "Ray.h"

Sphere::Sphere(const Vector& center, double radius, const Vector& albedo): Object(albedo), center(center), radius(radius) {}
Sphere::Sphere(const Vector& center, double radius, const AlbedoFunction& albedo): Object(albedo), center(center), radius(radius) {}

Object::IntersectResult Sphere::intersect(const Ray& ray) const {
	double a = 1;
	double b = 2 * ray.direction.dot(ray.origin - center);
	double c = (ray.origin - center).norm2() - radius * radius;
	double delta = b * b - 4 * a * c;
	if (delta < 0) { return {}; }
	double sqrt_delta = std::sqrt(delta);
	double t1 = (-b - sqrt_delta) / 2;
	double t2 = (-b + sqrt_delta) / 2;
	if (t2 < 0) { return {}; }
	double t = t1 > 0 ? t1 : t2;
	Vector impact = ray.origin + t * ray.direction;
	Vector normal = impact - center;
	Vector albedo = hasVariableAlbedo ? albedoFunction(impact) : this->albedo;
	return {.impact = impact, .normal = normal.normalized(), .distance = t, .albedo = albedo, .result = true};
}

Sphere& Sphere::mirror() {
	this->mirrors = true;
	return *this;
}

Sphere& Sphere::transparent(double opticalIndex) {
	this->isTransparent = true;
	this->opticalIndex = opticalIndex;
	return *this;
}

Sphere& Sphere::light(double power) {
	this->isLight = true;
	this->lightPower = power;
	return *this;
}

