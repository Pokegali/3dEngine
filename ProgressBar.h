//
// Created by remi on 24/01/25.
// Taken from https://stackoverflow.com/a/44555438
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <chrono>
#include <deque>

constexpr unsigned int UPDATE_INTERVAL = 10;
constexpr unsigned int UPDATE_AVERAGE = 10;

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
	explicit ProgressBar(unsigned int totalWork);
	void draw();
	ProgressBar& operator++();
	double stop();
	[[nodiscard]] double timeTaken() const;
	[[nodiscard]] unsigned int processed() const;

private:
	unsigned int totalWork;
	unsigned int callDiff;
	double lastUpdateTime = 0;
	unsigned int loopsWithoutUpdate = 0;
	unsigned int workDone = 0;
	Timer timer;
	unsigned int size = 50;
	std::deque<std::pair<double, unsigned int>> lastUpdates;

	static void clearConsoleLine() ;
};



#endif //PROGRESSBAR_H
