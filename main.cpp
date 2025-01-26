// #define _CRT_SECURE_NO_WARNINGS 1
#include <vector>
#include <chrono>
#include <iostream>

#include "stb_all.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector.h"
#include "ProgressBar.h"


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
	ProgressBar progressBar(HEIGHT * WIDTH);
#pragma omp parallel for default(none) schedule(static) shared(scene, origin, buffer, pixelTime, progressBar)
	for (int i = 0; i < HEIGHT; i++) {
		auto lineStartTime = get_clock();
		for (int j = 0; j < WIDTH; j++) {
			Vector pixel(j - static_cast<double>(WIDTH) / 2, -i + static_cast<double>(HEIGHT) / 2, -HEIGHT / (2 * tan(ALPHA / 2)));
			Vector color = scene.getColor(origin, pixel);
			buffer[(i * WIDTH + j) * 3 + 0] = adjustColor(color[0]);
			buffer[(i * WIDTH + j) * 3 + 1] = adjustColor(color[1]);
			buffer[(i * WIDTH + j) * 3 + 2] = adjustColor(color[2]);
			progressBar.update(i * WIDTH + j);
		}
		pixelTime += (get_clock() - lineStartTime) / 1ns;
	}
	pixelTime /= HEIGHT * WIDTH;
	long totalTime = (get_clock() - startTime) / 1ns;
	std::cout << std::endl << std::format("Temps moyen pour un pixel: {:.2f}µs (Total {:.1f}s)", static_cast<double>(pixelTime) / 1000, static_cast<double>(totalTime) / 1e9) << std::endl;
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

#pragma GCC diagnostic ignored "-Wstrict-overflow"
