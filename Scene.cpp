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

Vector Scene::getColor(const Ray& ray, int maxBounce) const {
	IntersectResult intersection = intersect(ray);
	if (maxBounce < 0 || !intersection.result) { return {100000, 0, 0}; }
	if (intersection.object->isTransparent) { return refractIntersection(ray, intersection, maxBounce); }
	if (intersection.object->mirrors) { return bounceIntersection(ray, intersection, maxBounce); }
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

Vector Scene::bounceIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const {
	Vector direction = ray.direction - 2 * ray.direction.dot(intersection.normal) * intersection.normal;
	Ray mirrorRay(intersection.impact + .001 * intersection.normal, direction);
	return getColor(mirrorRay, maxBounce - 1);
}

Vector Scene::refractIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const {
	double incidentNormalComponent = ray.direction.dot(intersection.normal);
	bool goingIn = incidentNormalComponent < 0;
	char sign = goingIn ? 1 : -1;
	Vector surfaceNormal = sign * intersection.normal;
	double n1 = 1;
	double n2 = intersection.object->opticalIndex;
	if (!goingIn) { std::swap(n1, n2); }
	double indexRatio = n1 / n2;
	double normalSquared = 1 - std::pow(indexRatio, 2) * (1 - std::pow(incidentNormalComponent, 2));
	if (normalSquared < 0) { return bounceIntersection(ray, intersection, maxBounce); }
	Vector tangent = indexRatio * (ray.direction - sign * incidentNormalComponent * surfaceNormal);
	Vector normal = -std::sqrt(normalSquared) * surfaceNormal;
	Ray refractedRay(intersection.impact - .001 * surfaceNormal, normal + tangent);
	Vector refractedColor = getColor(refractedRay, maxBounce - 1);
	double k0 = std::pow(n1 - n2, 2) / std::pow(n1 + n2, 2);
	double reflection = k0 + (1 - k0) * std::pow(1 - std::abs(incidentNormalComponent), 5);
	return reflection * bounceIntersection(ray, intersection, maxBounce) + (1 - reflection) * refractedColor;
}


