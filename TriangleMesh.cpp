#include "TriangleMesh.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <limits>
#include <string>


TriangleMesh::TriangleMesh(const Vector& albedo): Object(albedo) {}

void TriangleMesh::readOBJ(const char* obj) {
	std::ifstream stream(obj);
	std::string line;
	uint32_t curGroup = UINT_MAX;
	while (std::getline(stream, line)) {
		line.erase(line.find_last_not_of(" \r\t") + 1);
		if (line[0] == 'u' && line[1] == 's') {
			char grp[255];
			sscanf(line.c_str(), "usemtl %[^\n]\n", grp);
			curGroup++;
		} else if (line[0] == 'v' && line[1] == ' ') {
			Vector vec;
			Vector col;
			if (sscanf(line.c_str(), "v %lf %lf %lf %lf %lf %lf\n", &vec[0], &vec[1], &vec[2], &col[0], &col[1], &col[2]) == 6) {
				col[0] = std::clamp(col[0], 0., 1.);
				col[1] = std::clamp(col[1], 0., 1.);
				col[2] = std::clamp(col[2], 0., 1.);
				vertices.push_back(vec);
				vertexColors.push_back(col);
			} else {
				sscanf(line.c_str(), "v %lf %lf %lf\n", &vec[0], &vec[1], &vec[2]);
				vertices.push_back(vec);
			}
		} else if (line[0] == 'v' && line[1] == 'n') {
			Vector vec;
			sscanf(line.c_str(), "vn %lf %lf %lf\n", &vec[0], &vec[1], &vec[2]);
			normals.push_back(vec);
		} else if (line[0] == 'v' && line[1] == 't') {
			Vector vec;
			sscanf(line.c_str(), "vt %lf %lf\n", &vec[0], &vec[1]);
			uvs.push_back(vec);
		} else if (line[0] == 'f') {
			TriangleIndices triangle;
			uint32_t i0, i1, i2;
			uint32_t j0, j1, j2;
			uint32_t k0, k1, k2;
			triangle.group = curGroup;
			int offset;
			int read = sscanf(line.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u%n", &i0, &j0, &k0, &i1, &j1, &k1, &i2, &j2, &k2, &offset);
			if (read == 9) {
				triangle.vertexIndices = {i0 - 1, i1 - 1, i2 - 1};
				triangle.colorIndices = {j0 - 1, j1 - 1, j2 - 1};
				triangle.normalIndices = {k0 - 1, k1 - 1, k2 - 1};
			} else {
				read = sscanf(line.c_str(), "f %u/%u %u/%u %u/%u%n", &i0, &j0, &i1, &j1, &i2, &j2, &offset);
				if (read == 6) {
					triangle.vertexIndices = {i0 - 1, i1 - 1, i2 - 1};
					triangle.colorIndices = {j0 - 1, j1 - 1, j2 - 1};
				} else {
					read = sscanf(line.c_str(), "f %u %u %u%n", &i0, &i1, &i2, &offset);
					if (read == 3) {
						triangle.vertexIndices = {i0 - 1, i1 - 1, i2 - 1};
					} else {
						throw std::runtime_error(std::format("Unable to read vertex '{}'", line));
					}
				}
			}
			triangles.push_back(triangle);
		}
	}
	stream.close();
	buildBoundingBox();
}

bool BoundingBox::intersect(const Ray& ray) const {
	std::array<double, 3> minT {};
	std::array<double, 3> maxT {};
	Vector om = min - ray.origin;
	Vector oM = max - ray.origin;
	for (uint32_t i = 0; i < 3; i++) {
		double inter1 = om[i] / ray.direction[i];
		double inter2 = oM[i] / ray.direction[i];
		minT[i] = std::min(inter1, inter2);
		maxT[i] = std::max(inter1, inter2);
	}
	double minOfMax = *std::ranges::min_element(maxT);
	return minOfMax > 0 && minOfMax > *std::ranges::max_element(minT);
}

void TriangleMesh::buildBoundingBox() {
	Vector min = std::numeric_limits<double>::infinity() * Vector(1, 1, 1);
	Vector max = -min;
	for (const Vector& vertex: vertices) {
		for (uint32_t i = 0; i < 3; i++) {
			if (vertex[i] > max[i]) { max[i] = vertex[i]; }
			if (vertex[i] < min[i]) { min[i] = vertex[i]; }
		}
	}
	boundingBox.min = min;
	boundingBox.max = max;
}

void TriangleMesh::scaleTranslate(double scale, const Vector& translation) {
	for (Vector& vertex: vertices) {
		vertex = vertex * scale + translation;
	}
	buildBoundingBox();
}

Object::IntersectResult TriangleMesh::intersect(const Ray& ray) const {
	if (!boundingBox.intersect(ray)) {
		return {{}, {}, 0, false};
	}
	bool hasInter = false;
	double bestT = std::numeric_limits<double>::infinity();
	Vector bestNormal;
	Vector bestImpact;
	for (const auto& triangle : triangles) {
		const Vector& a = vertices[triangle.vertexIndices[0]];
		const Vector& b = vertices[triangle.vertexIndices[1]];
		const Vector& c = vertices[triangle.vertexIndices[2]];
		Vector e1 = b - a;
		Vector e2 = c - a;
		Vector ao = ray.origin - a;
		Vector aoCrossU = ao.cross(ray.direction);
		Vector normal = e1.cross(e2);
		double invDet = 1. / ray.direction.dot(normal);
		double beta = -e2.dot(aoCrossU) * invDet;
		if (beta < 0 || beta > 1) { continue; }
		double gamma = e1.dot(aoCrossU) * invDet;
		if (gamma < 0 || gamma > 1) { continue; }
		double alpha = 1 - beta - gamma;
		if (alpha < 0) { continue; }
		double t = -ao.dot(normal) * invDet;
		if (t < 0 || t > bestT) { continue; }
		hasInter = true;
		bestT = t;
		bestImpact = ray.origin + t * ray.direction;
		bestNormal = normal;
	}
	return {bestImpact, bestNormal, bestT, hasInter};
}
