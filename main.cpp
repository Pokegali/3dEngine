#include <vector>
#include <chrono>
#include <iostream>

#include "stb_all.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector.h"
#include "ProgressBar.h"


constexpr double ALPHA = 60 * M_PI / 180;
constexpr int HEIGHT = 512;
constexpr int WIDTH = 512;

uint8_t adjustColor(double color) {
	return static_cast<uint8_t>(std::min(255., std::pow(color, 1 / 2.2)));
}

constexpr auto get_clock = std::chrono::high_resolution_clock::now;

void drawScene(const Scene& scene, const Camera& camera, uint8_t* buffer) {
	using std::chrono_literals::operator ""ns;
	long pixelTime = 0;
	auto startTime = get_clock();
	ProgressBar progressBar(HEIGHT * WIDTH);
#pragma omp parallel for default(none) schedule(dynamic) shared(scene, camera, buffer, pixelTime, progressBar)
	for (int i = 0; i < HEIGHT; i++) {
		auto lineStartTime = get_clock();
		for (int j = 0; j < WIDTH; j++) {
			Vector pixel(j - static_cast<double>(WIDTH) / 2, -i + static_cast<double>(HEIGHT) / 2, HEIGHT / (2 * tan(ALPHA / 2)));
			Vector color = scene.getColor(camera, pixel, 55);
			buffer[(i * WIDTH + j) * 3 + 0] = adjustColor(color[0]);
			buffer[(i * WIDTH + j) * 3 + 1] = adjustColor(color[1]);
			buffer[(i * WIDTH + j) * 3 + 2] = adjustColor(color[2]);
			++progressBar;
		}
		pixelTime += (get_clock() - lineStartTime) / 1ns;
	}
	pixelTime /= HEIGHT * WIDTH;
	long totalTime = (get_clock() - startTime) / 1ns;
	std::cout << std::format("\nTemps moyen pour un rayon: {:.2f}µs (Total {:.1f}s)", static_cast<double>(pixelTime) / RAYS_PER_PIXEL / 1000, static_cast<double>(totalTime) / 1e9) << std::endl;
}

int main() {
	double angle = -10 * M_PI / 180;
	Camera camera({0, 10, 55}, {0, std::sin(angle), -std::cos(angle)}, {0, std::cos(angle), std::sin(angle)});
	Scene scene;

	const Sphere spheres[] = {
		Sphere(Vector(20, 20, 40), 5, Vector()).light(2e10),
		Sphere(Vector(15, -18, 3), 2, Vector(.5, .2, .9)),
		Sphere(Vector(0, -10020, 0), 10000, .2 * Vector(1, 1, 1)),
		Sphere(Vector(0, +10040, 0), 10000, .2 * Vector(1, 1, 1)),
		Sphere(Vector(-10040, 0, 0), 10000, .2 * Vector(1, 1, 1)),
		Sphere(Vector(+10040, 0, 0), 10000, .2 * Vector(1, 1, 1)),
		Sphere(Vector(0, 0, -10030), 10000, .2 * Vector(1, 1, 1)),
		Sphere(Vector(0, 0, +10070), 10000, .2 * Vector(1, 1, 1))
	};

	for (const Sphere& sphere: spheres) { scene.addSphere(&sphere); }

	auto* cobalion = new TriangleMesh(Vector(.9, .05, .05));
	cobalion->readOBJ("../objects/Cobalion/Cobalion.obj");
	cobalion->loadTexture("../objects/Cobalion/Cobalion_Mouth.png");
	cobalion->loadTexture("../objects/Cobalion/Cobalion_BodyA.png");
	cobalion->loadTexture("../objects/Cobalion/Cobalion_BodyB.png");
	cobalion->loadTexture("../objects/Cobalion/Cobalion_Eye.png");
	cobalion->rotate(M_PI / 3, 1);
	cobalion->scaleTranslate(4.5, Vector(0, -20, -7));
	cobalion->buildBvh();
	scene.addMesh(cobalion);

	auto* diancie = new TriangleMesh(Vector(.9, .4, .4));
	diancie->readOBJ("../objects/Diancie/Diancie.obj");
	diancie->loadTexture("../objects/Diancie/Diancie_BodyA.png");
	diancie->loadTexture("../objects/Diancie/Diancie_DiaEnv.png");
	diancie->loadTexture("../objects/Diancie/Diancie_Mouth.png");
	diancie->loadTexture("../objects/Diancie/Diancie_Eye.png");
	diancie->loadTexture("../objects/Diancie/Diancie_BodyB.png");
	diancie->loadTexture("../objects/Diancie/Diancie_DesukarnEnv.png");
	diancie->rotate(3 * M_PI / 2, 0);
	diancie->rotate(7 * M_PI / 6, 1);
	diancie->scaleTranslate(0.2, Vector(22, -15, 10));
	diancie->buildBvh();
	scene.addMesh(diancie);


	auto* image = new unsigned char[WIDTH * HEIGHT * 3];
	drawScene(scene, camera, image);
	stbi_write_png("image.png", WIDTH, HEIGHT, 3, image, 0);

	delete cobalion;
	delete diancie;
	delete[] image;

	return 0;
}

#pragma GCC diagnostic ignored "-Wstrict-overflow"
