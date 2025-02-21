//
// Created by remi on 14/02/25.
//

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include <array>
#include <vector>
#include <cstdint>
#include <climits>

#include "Object.h"
#include "Vector.h"

class TriangleIndices {
public:
	explicit TriangleIndices() = default;

	std::array<uint32_t, 3> vertexIndices {};
	std::array<uint32_t, 3> colorIndices {};
	std::array<uint32_t, 3> normalIndices {};

	// uint32_t vtxi, vtxj, vtxk; // indices within the vertex coordinates array
	// uint32_t uvi, uvj, uvk;  // indices within the uv coordinates array
	// uint32_t ni, nj, nk;  // indices within the normals array
	uint32_t group = UINT_MAX;       // face group
};

class BoundingBox {
public:
	[[nodiscard]] bool intersect(const Ray&) const;

	Vector min;
	Vector max;
};

class TriangleMesh: public Object {
public:
	explicit TriangleMesh(const Vector& albedo);

	void readOBJ(const char* obj);
	void buildBoundingBox();
	void scaleTranslate(double scale, const Vector& translation);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const override;

	std::vector<TriangleIndices> triangles;
	std::vector<Vector> vertices;
	std::vector<Vector> normals;
	std::vector<Vector> uvs;
	std::vector<Vector> vertexColors;
	BoundingBox boundingBox;
};

#endif //TRIANGLEMESH_H
