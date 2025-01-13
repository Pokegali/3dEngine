//
// Created by remi on 10/01/25.
//

#ifndef SCENE_H
#define SCENE_H
#include <vector>

#include "Sphere.h"


class Scene {
public:
	struct IntersectResult {
		Vector impact;
		Vector normal;
		const Sphere* object;
		bool result = false;
		double distance;
	};

	Scene(const Vector& source_pos, double source_intensity);
	void addSphere(const Sphere& sphere);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const;
	[[nodiscard]] Vector getColor(const Ray& ray) const;

	std::vector<Sphere> objects;
	Vector source_pos;
	double source_intensity;
};



#endif //SCENE_H
