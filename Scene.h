//
// Created by remi on 10/01/25.
//

#ifndef SCENE_H
#define SCENE_H
#include <array>
#include <random>
#include <vector>

#include "Sphere.h"
#include "TriangleMesh.h"

constexpr int RAYS_PER_PIXEL = 128;
constexpr int MAX_BOUNCE = 5;

static std::array<std::default_random_engine, 4> engines;
static std::uniform_real_distribution<> uniform(0, 1);

class Scene {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		const Object* object = nullptr;
		bool result = false;
		double distance = 0;
	};

	Scene();
	void addSphere(const Sphere*);
	void addMesh(const TriangleMesh*);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;
	[[nodiscard]] Vector getColor(const Ray& ray, int maxBounce, bool isIndirect = false) const;
	[[nodiscard]] Vector getColor(const Vector& origin, const Vector& pixel, double focusDistance) const;

	std::vector<const Object*> objects;
	const Sphere* lightSource = nullptr;

private:
	[[nodiscard]] Vector bounceIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
	[[nodiscard]] Vector refractIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
};



#endif //SCENE_H
