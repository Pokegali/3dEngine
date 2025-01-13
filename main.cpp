// #define _CRT_SECURE_NO_WARNINGS 1
#include <vector>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"
#include "stb_image.h"
#include "Vector.h"


constexpr double alpha = 60 * std::numbers::pi / 180;


unsigned char adjustColor(double color) {
	return static_cast<unsigned char>(std::min(255., std::pow(color, 1 / 2.2)));
}

void drawScene(const Scene& scene, const Vector& origin, int width, int height, unsigned char* buffer) {
#pragma omp parallel for
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			Vector u(j - static_cast<double>(width) / 2, -i + static_cast<double>(height) / 2, -width / (2 * tan(alpha / 2)));
			Ray ray(origin, u.normalized());
			Vector color = scene.getColor(ray);
			buffer[(i * width + j) * 3 + 0] = adjustColor(color[0]);
			buffer[(i * width + j) * 3 + 1] = adjustColor(color[1]);
			buffer[(i * width + j) * 3 + 2] = adjustColor(color[2]);
		}
	}
}

int main() {
	auto start = std::chrono::high_resolution_clock::now();

	Vector origin(0, 0, 55);
	Scene scene(Vector(10, 20, 40), 2e10);
	scene.addSphere(Sphere(Vector(0, -5, 0), 5, Vector(.5, .2, .9)));

	scene.addSphere(Sphere(Vector(0, -10020, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, +10040, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(-10040, 0, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(+10040, 0, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, 0, -10030), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, 0, +10070), 10000, Vector(.5, .5, .5)));

	int width = 512;
	int height = 512;

	unsigned char image[width * height * 3];
	drawScene(scene, origin, width, height, image);
	stbi_write_png("image.png", width, height, 3, image, 0);

	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Temps écoulé: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start) << std::endl;

	return 0;
}
