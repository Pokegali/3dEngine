//
// Created by remi on 24/01/25.
// Taken from https://stackoverflow.com/a/44555438
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <chrono>
#include <deque>

constexpr uint32_t UPDATE_INTERVAL = 10;
constexpr uint32_t UPDATE_AVERAGE = 10;

class Timer {
public:
	Timer() = default;
	void start();
	double stop();
	[[nodiscard]] double accumulated() const;
	[[nodiscard]] double lap() const;
	void reset();

private:
	typedef std::chrono::high_resolution_clock clock;
	typedef std::chrono::duration<double> second;

	std::chrono::time_point<clock> startTime;
	double accumulatedTime = 0;
	bool running = false;
};


class ProgressBar {
public:
	explicit ProgressBar(uint32_t totalWork);
	void draw();
	ProgressBar& operator++();
	double stop();
	[[nodiscard]] double timeTaken() const;
	[[nodiscard]] uint32_t processed() const;

private:
	uint32_t totalWork;
	uint32_t callDiff;
	double lastUpdateTime = 0;
	uint32_t loopsWithoutUpdate = 0;
	uint32_t workDone = 0;
	Timer timer;
	uint32_t size = 50;
	std::deque<std::pair<double, uint32_t>> lastUpdates;

	static void clearConsoleLine() ;
};



#endif //PROGRESSBAR_H
