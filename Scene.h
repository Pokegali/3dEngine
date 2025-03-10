//
// Created by remi on 10/01/25.
//

#ifndef SCENE_H
#define SCENE_H
#include <array>
#include <random>
#include <vector>

#include "Config.h"
#include "Sphere.h"
#include "TriangleMesh.h"

static std::array<std::default_random_engine, 4> engines;
static std::uniform_real_distribution<> uniform(0, 1);

struct Camera {
	Vector origin;
	Vector front;
	Vector up;
	Vector right = front.cross(up);

	void rotate(double angleRad, uint32_t axis);
};

class Scene {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		const Object* object = nullptr;
		double distance = 0;
		Vector albedo;
		bool result = false;
	};

	Scene();
	void addSphere(const Sphere*);
	void addMesh(const TriangleMesh*);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;
	[[nodiscard]] Vector getColor(const Ray& ray, int maxBounce, bool isIndirect = false) const;
	[[nodiscard]] Vector getColor(const Camera& camera, const Vector& pixel, const Config& config) const;

	std::vector<const Object*> objects;
	const Sphere* lightSource = nullptr;

private:
	[[nodiscard]] Vector bounceIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
	[[nodiscard]] Vector refractIntersection(const Ray& ray, const IntersectResult& intersection, int maxBounce) const;
};



#endif //SCENE_H
