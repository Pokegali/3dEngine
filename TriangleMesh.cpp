#include "TriangleMesh.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <stack>
#include <string>

#include "stb_image.h"

TriangleMesh::TriangleMesh(const Vector& albedo): Object(albedo) {}
TriangleMesh::~TriangleMesh() { delete rootBvh; }

// Adapted from https://pastebin.com/CAgp9r15
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
}

void TriangleMesh::loadTexture(const char* fileName) {
	int width, height, channels;
	uint8_t* textureData = stbi_load(fileName, &width, &height, &channels, STBI_rgb);
	Texture texture {{}, width, height};
	texture.data.reserve(width * height * 3);
	std::transform(textureData, textureData + width * height * 3, std::back_inserter(texture.data), [](const uint8_t& x) { return std::pow(static_cast<double>(x) / 255, 2.2); });
	textures.push_back(texture);
	stbi_image_free(textureData);
}

void TriangleMesh::computeTriangleBarycenters() {
	for (TriangleIndices& triangle: triangles) {
		triangle.barycenter = (vertices[triangle.vertexIndices[0]] + vertices[triangle.vertexIndices[1]] + vertices[triangle.vertexIndices[2]]) / 3;
	}
}

BoundingBox::IntersectResult BoundingBox::intersect(const Ray& ray) const {
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
	double minOfMax = std::min(maxT[0], std::min(maxT[1], maxT[2]));
	double maxOfMin = std::max(minT[0], std::max(minT[1], minT[2]));
	return {maxOfMin, minOfMax > 0 && minOfMax > maxOfMin};
}

Vector BoundingBox::extent() const {
	return max - min;
}

BoundingVolumeHierarchy::BoundingVolumeHierarchy(uint32_t start, uint32_t end, const TriangleMesh& mesh): rangeStart(start), rangeEnd(end), mesh(mesh) {}
BoundingVolumeHierarchy::~BoundingVolumeHierarchy() {
	delete leftChild;
	delete rightChild;
}

void BoundingVolumeHierarchy::buildBoundingBox() {
	Vector min = std::numeric_limits<double>::infinity() * vec111;
	Vector max = -min;
	for (uint32_t triangle = rangeStart; triangle < rangeEnd; triangle++) {
		for (const uint32_t& vertex: mesh.triangles[triangle].vertexIndices) {
			for (uint32_t i = 0; i < 3; i++) {
				const Vector& coordinates = mesh.vertices[vertex];
				if (coordinates[i] > max[i]) { max[i] = coordinates[i]; }
				if (coordinates[i] < min[i]) { min[i] = coordinates[i]; }
			}
		}
	}
	boundingBox.min = min;
	boundingBox.max = max;
}

Object::IntersectResult BoundingVolumeHierarchy::intersect(const Ray& ray) const {
	bool hasInter = false;
	double bestT = std::numeric_limits<double>::infinity();
	Vector bestNormal;
	Vector bestImpact;
	Vector bestAlbedo;
	for (uint32_t index = rangeStart; index < rangeEnd; ++index) {
		const TriangleIndices& triangle = mesh.triangles[index];
		const Vector& a = mesh.vertices[triangle.vertexIndices[0]];
		const Vector& b = mesh.vertices[triangle.vertexIndices[1]];
		const Vector& c = mesh.vertices[triangle.vertexIndices[2]];
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
		Vector correctedNormal = mesh.normals[triangle.normalIndices[0]] * alpha + mesh.normals[triangle.normalIndices[1]] * beta + mesh.normals[triangle.normalIndices[2]] * gamma;
		if (!mesh.textures.empty()) {
			Vector colorPosition = mesh.uvs[triangle.colorIndices[0]] * alpha + mesh.uvs[triangle.colorIndices[1]] * beta + mesh.uvs[triangle.colorIndices[2]] * gamma;
			const TriangleMesh::Texture& texture = mesh.textures[triangle.group];
			uint32_t colorU = std::fmod(colorPosition[0] + 1000, 1) * texture.width;
			uint32_t colorV = (1 - std::fmod(colorPosition[1] + 1000, 1)) * texture.height;
			uint32_t indexInTexture = 3 * (colorV * texture.width + colorU);
			bestAlbedo = Vector(texture.data[indexInTexture], texture.data[indexInTexture + 1], texture.data[indexInTexture + 2]);
		} else {
			bestAlbedo = {};
		}
		hasInter = true;
		bestT = t;
		bestImpact = ray.origin + t * ray.direction;
		bestNormal = correctedNormal;
	}
	return {.impact = bestImpact, .normal = bestNormal, .distance = bestT, .albedo = bestAlbedo, .result = hasInter};
}

void TriangleMesh::buildBvh() {
	computeTriangleBarycenters();
	rootBvh = new BoundingVolumeHierarchy(0, triangles.size(), *this);
	buildBvh(rootBvh);
}

void TriangleMesh::buildBvh(BoundingVolumeHierarchy* bvh) {
	bvh->buildBoundingBox();
	if (bvh->rangeEnd - bvh->rangeStart <= 4) { return; }
	std::array<double, 3> extent = bvh->boundingBox.extent().getCoordinates();
	long longestDirection = std::distance(extent.begin(), std::ranges::max_element(extent));
	double limit = (bvh->boundingBox.max[longestDirection] + bvh->boundingBox.min[longestDirection]) / 2;
	auto pivot = std::partition(triangles.begin() + bvh->rangeStart, triangles.begin() + bvh->rangeEnd, [longestDirection, limit](const TriangleIndices& triangle) { return triangle.barycenter[longestDirection] <= limit; });
	long pivotIndex = std::distance(triangles.begin(), pivot);
	if (pivotIndex == bvh->rangeStart || pivotIndex == bvh->rangeEnd) { return; }
	auto* left = new BoundingVolumeHierarchy(bvh->rangeStart, pivotIndex, *this);
	auto* right = new BoundingVolumeHierarchy(pivotIndex, bvh->rangeEnd, *this);
	bvh->leftChild = left;
	bvh->rightChild = right;
	buildBvh(left);
	buildBvh(right);
}

void TriangleMesh::scaleTranslate(double scale, const Vector& translation) {
	for (Vector& vertex: vertices) {
		vertex = vertex * scale + translation;
	}
}

void TriangleMesh::rotate(double angleRad, uint32_t axis) {
	for (Vector& vertex: vertices) {
		vertex.rotate(angleRad, axis);
	}
}

Object::IntersectResult TriangleMesh::intersect(const Ray& ray) const {
	if (!rootBvh->boundingBox.intersect(ray).result) { return {}; }
	std::stack<const BoundingVolumeHierarchy*> stack;
	IntersectResult bestIntersect {.impact = {}, .normal = {}, .distance = std::numeric_limits<double>::infinity(), .albedo = {}};
	stack.push(rootBvh);
	BoundingBox::IntersectResult intersect;
	while (!stack.empty()) {
		const BoundingVolumeHierarchy* bvh = stack.top();
		stack.pop();
		if (bvh->leftChild != nullptr) {
			if (intersect = bvh->leftChild->boundingBox.intersect(ray); intersect.result && intersect.distance < bestIntersect.distance) { stack.push(bvh->leftChild); }
			if (intersect = bvh->rightChild->boundingBox.intersect(ray); intersect.result && intersect.distance < bestIntersect.distance) { stack.push(bvh->rightChild); }
		} else {
			IntersectResult result = bvh->intersect(ray);
			if (result.distance < bestIntersect.distance) { bestIntersect = result; }
		}
	}
	return bestIntersect;
}
