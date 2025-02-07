//
// Created by remi on 24/01/25.
// Taken from https://stackoverflow.com/a/44555438
//

#include "ProgressBar.h"

#include <iostream>
#include <iterator>
#include <omp.h>

void Timer::start() {
	if (running) { throw std::runtime_error("Timer was already started"); }
	running = true;
	startTime = clock::now();
}

double Timer::stop() {
	if (!running) { throw std::runtime_error("Timer is not running"); }
	accumulatedTime += lap();
	running = false;
	return accumulatedTime;
}

double Timer::accumulated() const {
	if (running) { throw std::runtime_error("Timer is still running"); }
	return accumulatedTime;
}

double Timer::lap() const {
	if (!running) { throw std::runtime_error("Timer is not running"); }
	return std::chrono::duration_cast<second>(clock::now() - startTime).count();
}

void Timer::reset() {
	accumulatedTime = 0;
	running = false;
}

ProgressBar::ProgressBar(int totalWork): totalWork(totalWork), callDiff(totalWork / 400) {
	clearConsoleLine();
	timer.start();
}

void ProgressBar::clearConsoleLine() { std::cout << "\r\033[2K" << std::flush; }

void ProgressBar::update(int workDone) {
#ifdef NOPROGRESS
	return;
#endif
	if (omp_get_thread_num() != 0) { return; }
	this->workDone = workDone;
	double currentTime = timer.lap();
	if (currentTime - lastUpdateTime < .1 && workDone != totalWork) { return; }
	lastUpdateTime = currentTime;
	double percent = static_cast<double>(workDone) * omp_get_num_threads() * 100 / totalWork;
	auto full = static_cast<unsigned int>(size * percent / 100);
	auto more = static_cast<unsigned int>(size * percent * 8 / 100 - full * 8);
	if (percent > 100) { percent = 100; }
	double secondsLeft = currentTime / percent * (100 - percent);
	int minutesLeft = static_cast<int>(secondsLeft / 60);
	secondsLeft -= minutesLeft * 60;
	static const unsigned char characters[] = {0x8f, 0x8e, 0x8d, 0x8c, 0x8b, 0x8a, 0x89, 0x88};
	std::cout << "\r\033[2K[";
	std::fill_n(std::ostream_iterator<std::string>(std::cout), full, "\xe2\x96\x88");
	if (more != 0) { std::cout << "\xe2\x96" << characters[more-1]; }
	else if (percent != 100) { std::cout << ' '; }
	std::cout << std::string(size - full - 1, ' ')
			  << "] ("
			  << percent << "% - "
			  << (minutesLeft ? std::format("{}m {:.1f}", minutesLeft, secondsLeft) : std::format("{:.1f}", secondsLeft))
			  << "s - "
			  << omp_get_num_threads() <<  " threads - "
	          << std::fixed << std::setprecision(0) << workDone / currentTime << " it/s)" << std::flush;
}

ProgressBar& ProgressBar::operator++() {
	if (omp_get_thread_num() != 0) { return *this; }
	workDone++;
	update(workDone);
	return *this;
}

double ProgressBar::stop() {
	clearConsoleLine();
	timer.stop();
	return timer.accumulated();
}

double ProgressBar::timeTaken() const {
	return timer.accumulated();
}

int ProgressBar::processed() const {
	return workDone;
}
