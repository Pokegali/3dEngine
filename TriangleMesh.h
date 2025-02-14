//
// Created by remi on 14/02/25.
//

#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

//
// Created by remi on 14/02/25.
//

#include <vector>

#include "Object.h"
#include "Vector.h"

class TriangleIndices {
public:
	explicit TriangleIndices(int vtxi = -1, int vtxj = -1, int vtxk = -1, int ni = -1, int nj = -1, int nk = -1, int uvi = -1, int uvj = -1, int uvk = -1, int group = -1, bool added = false) : vtxi(vtxi), vtxj(vtxj), vtxk(vtxk), uvi(uvi), uvj(uvj), uvk(uvk), ni(ni), nj(nj), nk(nk), group(group) {};

	int vtxi, vtxj, vtxk; // indices within the vertex coordinates array
	int uvi, uvj, uvk;  // indices within the uv coordinates array
	int ni, nj, nk;  // indices within the normals array
	int group;       // face group
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

	std::vector<TriangleIndices> indices;
	std::vector<Vector> vertices;
	std::vector<Vector> normals;
	std::vector<Vector> uvs;
	std::vector<Vector> vertexColors;
	BoundingBox boundingBox;
};

#endif //TRIANGLEMESH_H
