//
// Created by remi on 24/01/25.
// Taken from https://stackoverflow.com/a/44555438
//

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <chrono>


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
	explicit ProgressBar(int totalWork);
	void update(int workDone);
	ProgressBar& operator++();
	double stop();
	[[nodiscard]] double timeTaken() const;
	[[nodiscard]] int processed() const;

private:
	int totalWork;
	int nextUpdate = 0;
	int callDiff;
	int workDone = 0;
	int oldPercent = 0;
	Timer timer;

	static void clearConsoleLine() ;
};



#endif //PROGRESSBAR_H
