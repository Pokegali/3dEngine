#include <vector>
#include <chrono>
#include <iostream>

#include "stb_all.h"
#include "Scene.h"
#include "Sphere.h"
#include "Vector.h"
#include "ProgressBar.h"
#include "Config.h"

uint8_t adjustColor(double color) {
	return static_cast<uint8_t>(std::min(255., std::pow(color, 1 / 2.2)));
}

constexpr auto get_clock = std::chrono::high_resolution_clock::now;

void drawScene(const Scene& scene, const Camera& camera, const Config& config, uint8_t* buffer) {
	using std::chrono_literals::operator ""ns;
	long pixelTime = 0;
	auto startTime = get_clock();
	ProgressBar progressBar(config.height * config.width);
#pragma omp parallel for default(none) schedule(dynamic) shared(scene, camera, config, buffer, pixelTime, progressBar)
	for (int i = 0; i < config.height; i++) {
		auto lineStartTime = get_clock();
		for (int j = 0; j < config.width; j++) {
			Vector pixel(j - static_cast<double>(config.width) / 2, -i + static_cast<double>(config.height) / 2, config.height / (2 * tan(config.alpha / 2)));
			Vector color = scene.getColor(camera, pixel, config);
			buffer[(i * config.width + j) * 3 + 0] = adjustColor(color[0]);
			buffer[(i * config.width + j) * 3 + 1] = adjustColor(color[1]);
			buffer[(i * config.width + j) * 3 + 2] = adjustColor(color[2]);
			++progressBar;
		}
		pixelTime += (get_clock() - lineStartTime) / 1ns;
	}
	pixelTime /= config.height * config.width;
	long totalTime = (get_clock() - startTime) / 1ns;
	std::cout << std::format("\nTemps moyen pour un rayon: {:.2f}µs (Total {:.1f}s)", static_cast<double>(pixelTime) / config.raysPerPixel / 1000, static_cast<double>(totalTime) / 1e9) << std::endl;
}

int main() {
	Config config {};
	readConfig("../params.cfg", config);

	Camera camera({-10, 10, 55}, {0, 0, -1}, {0, 1, 0});
	camera.rotate(-10 * M_PI / 180, 0);
	camera.rotate(-20 * M_PI / 180, 1);
	Scene scene;

	const Sphere spheres[] = {
		Sphere(Vector(20, 20, 40), 5, Vector()).light(2e10),
		Sphere(Vector(15, -18, 3), 2, Vector(.5, .2, .9)),
		Sphere(Vector(10, -17, 15), 3, Vector()).transparent(1.5),
		Sphere(Vector(-30, -17.5, -20), 2.5, Vector()).mirror(),
		Sphere(Vector(0, -10020, 0), 10000, AlbedoFunctions::checkerboard(1, 3, .2 * vec111, .1 * vec111)),
		Sphere(Vector(0, +10040, 0), 10000, .2 * vec111),
		Sphere(Vector(-10040, 0, 0), 10000, .2 * vec111),
		Sphere(Vector(+10040, 0, 0), 10000, .2 * vec111),
		Sphere(Vector(0, 0, -10030), 10000, .2 * vec111),
		Sphere(Vector(0, 0, +10070), 10000, .2 * vec111)
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


	auto* image = new unsigned char[config.width * config.height * 3];
	drawScene(scene, camera, config, image);
	stbi_write_png("image.png", config.width, config.height, 3, image, 0);

	delete cobalion;
	delete diancie;
	delete[] image;

	return 0;
}

#pragma GCC diagnostic ignored "-Wstrict-overflow"
