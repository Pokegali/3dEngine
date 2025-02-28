//
// Created by remi on 10/01/25.
//

#include "Sphere.h"

#include <cmath>

#include "Ray.h"

Sphere::Sphere(const Vector& center, double radius, const Vector& albedo): Object(albedo), center(center), radius(radius) {}

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
	Vector albedo = this->albedo;
	if (center[1] < -1000) {
		double col = std::ceil((std::round((std::cos(impact[0]) + 1) / 2) + std::round((std::cos(impact[2]) + 1) / 2)) / 2);
		col += std::ceil((std::round((std::cos(impact[0] + M_PI) + 1) / 2) + std::round((std::cos(impact[2] + M_PI) + 1) / 2)) / 2);
		albedo = {col, col, col};
		albedo = albedo * this->albedo;
	}
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

