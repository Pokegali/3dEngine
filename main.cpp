// #define _CRT_SECURE_NO_WARNINGS 1
#include <vector>
#include <chrono>
#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Ray.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector.h"


constexpr double ALPHA = 60 * std::numbers::pi / 180;
constexpr int HEIGHT = 512;
constexpr int WIDTH = 512;

unsigned char adjustColor(double color) {
	return static_cast<unsigned char>(std::min(255., std::pow(color, 1 / 2.2)));
}

constexpr auto get_clock = std::chrono::high_resolution_clock::now;

void drawScene(const Scene& scene, const Vector& origin, unsigned char* buffer) {
	using std::chrono_literals::operator ""ns;
	long pixelTime = 0;
	auto startTime = get_clock();
#pragma omp parallel for default(none) shared(scene, origin, buffer, pixelTime)
	for (int i = 0; i < HEIGHT; i++) {
		auto lineStartTime = get_clock();
		for (int j = 0; j < WIDTH; j++) {
			Vector u(j - static_cast<double>(WIDTH) / 2, -i + static_cast<double>(HEIGHT) / 2, -HEIGHT / (2 * tan(ALPHA / 2)));
			Ray ray(origin, u.normalized());
			Vector color = scene.getColor(ray, 20);
			buffer[(i * WIDTH + j) * 3 + 0] = adjustColor(color[0]);
			buffer[(i * WIDTH + j) * 3 + 1] = adjustColor(color[1]);
			buffer[(i * WIDTH + j) * 3 + 2] = adjustColor(color[2]);
		}
		pixelTime += (get_clock() - lineStartTime) / 1ns;
	}
	pixelTime /= HEIGHT * WIDTH;
	long totalTime = (get_clock() - startTime) / 1ns;
	std::cout << std::format("Temps moyen pour un pixel: {:.2f}µs (Total {:.0f}ms)", static_cast<double>(pixelTime) / 1000, static_cast<double>(totalTime) / 1e6) << std::endl;
}

int main() {
	Vector origin(0, 0, 55);
	Scene scene(Vector(10, 20, 40), 2e10);
	scene.addSphere(Sphere(Vector(0, -10, 0), 5, Vector(.5, .2, .9)).transparent(1.5));

	scene.addSphere(Sphere(Vector(0, -10020, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, +10040, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(-10040, 0, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(+10040, 0, 0), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, 0, -10030), 10000, Vector(.5, .5, .5)));
	scene.addSphere(Sphere(Vector(0, 0, +10070), 10000, Vector(.5, .5, .5)));

	unsigned char image[WIDTH * HEIGHT * 3];
	drawScene(scene, origin, image);
	stbi_write_png("image.png", WIDTH, HEIGHT, 3, image, 0);

	return 0;
}
