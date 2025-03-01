//
// Created by remi on 10/01/25.
//

#include "Scene.h"

#include <limits>
#include <cmath>
#include <iostream>
#include <omp.h>

#include "Config.h"

constexpr double EPSILON = 1e-6;

double getRandomUniform() {
	return uniform(engines[static_cast<uint32_t>(omp_get_thread_num())]);
}

std::pair<double, double> boxMuller(double stdDev) {
	double u1 = getRandomUniform() + 1e-16; // Or else it might return 0 at some point but only with -O0
	double u2 = getRandomUniform() + 1e-16;
	double r = std::sqrt(-2 * std::log(u1));
	return {r * std::cos(2 * M_PI * u2) * stdDev, r * std::sin(2 * M_PI * u2) * stdDev};
}

Vector cosRandomVector(const Vector& normal) {
	double r1 = getRandomUniform();
	double r2 = getRandomUniform();
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

void Camera::rotate(double angleRad, uint32_t axis) {
	front.rotate(angleRad, axis);
	up.rotate(angleRad, axis);
	right.rotate(angleRad, axis);
}

Scene::Scene() {
	for (uint32_t i = 0; i < 4; i++) { engines[i].seed(i); }
}

void Scene::addSphere(const Sphere* sphere) {
	objects.push_back(sphere);
	if (sphere->isLight) {
		lightSource = sphere;
	}
}

void Scene::addMesh(const TriangleMesh* mesh) {
	objects.push_back(mesh);
}


Scene::IntersectResult Scene::intersect(const Ray& ray) const {
	bool hasInter = false;
	double minT = std::numeric_limits<double>::infinity();
	Vector normal;
	Vector impact;
	Vector albedo;
	const Object* hitObject = nullptr;
	for (const Object* object: objects) {
		Object::IntersectResult intersect = object->intersect(ray);
		if (intersect.result) {
			hasInter = true;
			if (intersect.distance < minT) {
				normal = intersect.normal;
				impact = intersect.impact;
				minT = intersect.distance;
				albedo = intersect.albedo;
				hitObject = object;
			}
		}
	}
	return {.impact = impact, .normal = normal, .object = hitObject, .distance = minT, .albedo = albedo, .result = hasInter};
}

Vector Scene::getColor(const Ray& ray, int maxBounce, bool isIndirect) const {
	IntersectResult intersection = intersect(ray);
	if (maxBounce < 0 || !intersection.result) { return {0, 0, 0}; }
	if (intersection.object->isTransparent) { return refractIntersection(ray, intersection, maxBounce); }
	if (intersection.object->mirrors) { return bounceIntersection(ray, intersection, maxBounce); }
	if (intersection.object->isLight) {
		if (isIndirect) { return {0, 0, 0}; }
		double power = lightSource->lightPower / (4 * M_PI * M_PI * lightSource->radius * lightSource->radius);
		return {power, power, power};
	}
	Vector travel = lightSource->center - intersection.impact;
	Vector lightDirection = travel.normalized();
	Vector nPrime = cosRandomVector(-lightDirection);
	Vector randomLightPath = nPrime * lightSource->radius + lightSource->center - intersection.impact;
	double distance_2 = randomLightPath.norm2();
	Vector randomLightDirection = randomLightPath.normalized();
	Ray shadowRay(intersection.impact + intersection.normal * EPSILON / 10, randomLightDirection);
	IntersectResult shadowIntersect = intersect(shadowRay);
	Vector directContribution;
	if (shadowIntersect.result && shadowIntersect.distance * shadowIntersect.distance < distance_2 - 100 * EPSILON) {
		directContribution = Vector(0, 0, 0);
	} else {
		double px = std::max(1e-12, -lightDirection.dot(nPrime));
		directContribution =
			lightSource->lightPower / (4 * M_PI * M_PI) *
			intersection.albedo *
			std::max(0., intersection.normal.dot(randomLightDirection)) / px *
			std::max(0., nPrime.dot(-randomLightDirection)) / distance_2;
	}
	Vector indirectContribution = getColor(Ray(intersection.impact + EPSILON * intersection.normal, cosRandomVector(intersection.normal)), maxBounce - 1, true) * intersection.albedo;
	return indirectContribution + directContribution;
}

Vector Scene::getColor(const Camera& camera, const Vector& pixel, const Config& config) const {
	Vector color;
	for (int repeat = 0; repeat < config.raysPerPixel; repeat++) {
		auto [dxPixel, dyPixel] = boxMuller(.5);
		auto [dxCamera, dyCamera] = boxMuller(.5);
		Vector u = (pixel + Vector(.5 + dxPixel, -.5 - dyPixel, 0) - camera.origin).normalized();
		u = u[0] * camera.right + u[1] * camera.up + u[2] * camera.front;
		Vector newOrigin = camera.origin + Vector(dxCamera, dyCamera, 0);
		Vector destination = camera.origin + u / u.dot(camera.front) * config.focusDistance;
		Vector newDirection = destination - newOrigin;
		Ray ray(newOrigin, newDirection.normalized());
		color += getColor(ray, config.maxBounce);
	}
	return color / config.raysPerPixel;
}

Vector Scene::bounceIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const {
	Vector direction = ray.direction - 2 * ray.direction.dot(intersection.normal) * intersection.normal;
	Ray mirrorRay(intersection.impact + EPSILON * intersection.normal, direction);
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
	if (getRandomUniform() < reflection) {
		return bounceIntersection(ray, intersection, maxBounce - 1);
	}
	double normalSquared = 1 - std::pow(indexRatio, 2) * (1 - std::pow(incidentNormalComponent, 2));
	if (normalSquared < 0) { return bounceIntersection(ray, intersection, maxBounce); }
	Vector tangent = indexRatio * (ray.direction - sign * incidentNormalComponent * surfaceNormal);
	Vector normal = -std::sqrt(normalSquared) * surfaceNormal;
	Ray refractedRay(intersection.impact - EPSILON * surfaceNormal, normal + tangent);
	return getColor(refractedRay, maxBounce);
}


