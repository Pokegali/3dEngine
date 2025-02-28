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

class TriangleMesh;

class TriangleIndices {
public:
	explicit TriangleIndices() = default;

	std::array<uint32_t, 3> vertexIndices {};
	std::array<uint32_t, 3> colorIndices {};
	std::array<uint32_t, 3> normalIndices {};
	Vector barycenter;

	// uint32_t vtxi, vtxj, vtxk; // indices within the vertex coordinates array
	// uint32_t uvi, uvj, uvk;  // indices within the uv coordinates array
	// uint32_t ni, nj, nk;  // indices within the normals array
	uint32_t group = UINT_MAX;       // face group
};

class BoundingBox {
public:
	struct IntersectResult {
		double distance = 0;
		bool result = false;
	};

	[[nodiscard]] IntersectResult intersect(const Ray&) const;
	[[nodiscard]] Vector extent() const;

	Vector min;
	Vector max;
};


class BoundingVolumeHierarchy {
public:
	BoundingVolumeHierarchy(uint32_t start, uint32_t end, const TriangleMesh& mesh);

	void buildBoundingBox();
	[[nodiscard]] Object::IntersectResult intersect(const Ray& ray) const;

	const BoundingVolumeHierarchy* leftChild = nullptr;
	const BoundingVolumeHierarchy* rightChild = nullptr;
	BoundingBox boundingBox;
	uint32_t rangeStart;
	uint32_t rangeEnd;
	const TriangleMesh& mesh;
};


class TriangleMesh: public Object {
public:
	struct Texture {
		std::vector<double> data;
		int width;
		int height;
	};

	explicit TriangleMesh(const Vector& albedo);

	void readOBJ(const char* obj);
	void loadTexture(const char* fileName);
	void computeTriangleBarycenters();
	void buildBvh();
	void scaleTranslate(double scale, const Vector& translation);
	void rotate(double angleRad);
	[[nodiscard]] IntersectResult intersect(const Ray& ray) const override;

	std::vector<TriangleIndices> triangles;
	std::vector<Vector> vertices;
	std::vector<Vector> normals;
	std::vector<Vector> uvs;
	std::vector<Vector> vertexColors;
	std::vector<Texture> textures;
	BoundingVolumeHierarchy* rootBvh = nullptr;

private:
	void buildBvh(BoundingVolumeHierarchy* bvh);
};

#endif //TRIANGLEMESH_H
