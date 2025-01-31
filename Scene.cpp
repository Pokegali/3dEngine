//
// Created by remi on 10/01/25.
//

#include "Scene.h"

#include <limits>
#include <cmath>
#include <omp.h>

std::pair<double, double> boxMuller(double stdDev) {
	double u1 = uniform(engines[omp_get_thread_num()]);
	double u2 = uniform(engines[omp_get_thread_num()]);
	double r = std::sqrt(-2 * std::log(u1));
	return {r * std::cos(2 * M_PI * u2) * stdDev, r * std::sin(2 * M_PI * u2) * stdDev};
}

Vector cosRandomVector(const Vector& normal) {
	double r1 = uniform(engines[omp_get_thread_num()]);
	double r2 = uniform(engines[omp_get_thread_num()]);
	double sr2 = std::sqrt(1 - r2);
	Vector direction {
		std::cos(2 * M_PI * r1) * sr2,
		std::sin(2 * M_PI * r1) * sr2,
		std::sqrt(r2)
	};
	Vector tangent;
	if (std::abs(normal[0]) <= std::abs(normal[1]) && std::abs(normal[0]) <= std::abs(normal[2])) {
		tangent = Vector(0, normal[2], -normal[1]).normalized();
	} else if (std::abs(normal[1]) <= std::abs(normal[0]) && std::abs(normal[1]) <= std::abs(normal[2])) {
		tangent = Vector(normal[2], 0, -normal[0]).normalized();
	} else {
		tangent = Vector(normal[1], -normal[0], 0).normalized();
	}
	Vector tangent2 = normal.cross(tangent).normalized();
	return direction[2] * normal + direction[0] * tangent + direction[1] * tangent2;
}

Scene::Scene(const Vector& source_pos, double source_intensity): source_pos(source_pos), source_intensity(source_intensity) {
	for (unsigned int i = 0; i < 4; i++) { engines[i].seed(i); }
}

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
	if (maxBounce < 0 || !intersection.result) { return {0, 0, 0}; }
	if (intersection.object->isTransparent) { return refractIntersection(ray, intersection, maxBounce); }
	if (intersection.object->mirrors) { return bounceIntersection(ray, intersection, maxBounce); }
	Vector travel = source_pos - intersection.impact;
	double distance_2 = travel.norm2();
	Vector lightDirection = travel.normalized();
	Ray shadowRay(intersection.impact + lightDirection * .001, lightDirection);
	IntersectResult shadowIntersect = intersect(shadowRay);
	Vector directContribution = shadowIntersect.result && shadowIntersect.distance * shadowIntersect.distance < distance_2
		? Vector(0, 0, 0)
		: source_intensity * std::max(0., intersection.normal.dot(lightDirection)) / (4 * M_PI * M_PI * distance_2) * intersection.object->albedo;
	Vector indirectContribution = getColor(Ray(intersection.impact + .001 * intersection.normal, cosRandomVector(intersection.normal)), maxBounce - 1) * intersection.object->albedo;
	return indirectContribution + directContribution;
}

Vector Scene::getColor(const Vector& origin, const Vector& pixel) const {
	Vector color;
	for (int repeat = 0; repeat < RAYS_PER_PIXEL; repeat++) {
		auto [n1, n2] = boxMuller(.5);
		Vector u = pixel + Vector(.5 + n1, -.5 - n2, 0);
		Ray ray(origin, u.normalized());
		color += getColor(ray, 10);
	}
	return color / RAYS_PER_PIXEL;
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
	double k0 = std::pow(n1 - n2, 2) / std::pow(n1 + n2, 2);
	double reflection = k0 + (1 - k0) * std::pow(1 - std::abs(incidentNormalComponent), 5);
	if (uniform(engines[omp_get_thread_num()]) < reflection) {
		return bounceIntersection(ray, intersection, maxBounce - 1);
	}
	double normalSquared = 1 - std::pow(indexRatio, 2) * (1 - std::pow(incidentNormalComponent, 2));
	if (normalSquared < 0) { return bounceIntersection(ray, intersection, maxBounce); }
	Vector tangent = indexRatio * (ray.direction - sign * incidentNormalComponent * surfaceNormal);
	Vector normal = -std::sqrt(normalSquared) * surfaceNormal;
	Ray refractedRay(intersection.impact - .001 * surfaceNormal, normal + tangent);
	return getColor(refractedRay, maxBounce);
}


