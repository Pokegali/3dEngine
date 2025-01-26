//
// Created by remi on 10/01/25.
//

#ifndef SCENE_H
#define SCENE_H
#include <array>
#include <random>
#include <vector>

#include "Sphere.h"

constexpr int RAYS_PER_PIXEL = 1024;

static std::array<std::default_random_engine, 4> engines;
static std::uniform_real_distribution<> uniform(0, 1);

class Scene {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		const Sphere* object = nullptr;
		bool result = false;
		double distance = 0;
	};

	Scene(const Vector& source_pos, double source_intensity);
	void addSphere(const Sphere& sphere);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;
	[[nodiscard]] Vector getColor(const Ray& ray, int maxBounce) const;
	[[nodiscard]] Vector getColor(const Vector& origin, const Vector& pixel) const;

	std::vector<Sphere> objects;
	Vector source_pos;
	double source_intensity;

private:
	[[nodiscard]] Vector bounceIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
	[[nodiscard]] Vector refractIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
};



#endif //SCENE_H
