//
// Created by remi on 10/01/25.
//

#include "Scene.h"

#include <limits>
#include <cmath>

Scene::Scene(const Vector& source_pos, double source_intensity): source_pos(source_pos), source_intensity(source_intensity) {}

void Scene::addSphere(const Sphere& sphere) {
	objects.push_back(sphere);
}

Scene::IntersectResult Scene::intersect(const Ray& ray) const {
	bool hasInter = false;
	double minT = std::numeric_limits<double>::infinity();
	Vector normal;
	Vector impact;
	const Sphere* hitObject = nullptr;
	for (const Sphere& object: objects) {
		Sphere::IntersectResult intersect = object.intersect(ray);
		if (intersect.result) {
			hasInter = true;
			if (intersect.distance < minT) {
				normal = intersect.normal;
				impact = intersect.impact;
				minT = intersect.distance;
				hitObject = &object;
			}
		}
	}
	return {impact, normal, hitObject, hasInter, minT};
}

Vector Scene::getColor(const Ray& ray) const {
	IntersectResult intersection = intersect(ray);
	if (intersection.result) {
		Vector travel = source_pos - intersection.impact;
		double distance_2 = travel.norm2();
		Vector lightDirection = travel.normalized();
		Ray shadowRay(intersection.impact + lightDirection * .001, lightDirection);
		IntersectResult shadowIntersect = intersect(shadowRay);
		if (shadowIntersect.result && shadowIntersect.distance * shadowIntersect.distance < distance_2) {
			return {0, 0, 0};
		}
		return source_intensity * std::max(0., intersection.normal.dot(lightDirection)) / (4 * std::numbers::pi * std::numbers::pi * distance_2) * intersection.object->albedo;
	}
	return {0, 0, 0};
}
