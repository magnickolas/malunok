#include "motion_params/timer.h"
#include <cstdint>

AbstractTimer::~AbstractTimer() {}

SharedTimer::SharedTimer()
    : begin_time_point_(std::chrono::steady_clock::now()),
      time_point_(begin_time_point_) {}

void SharedTimer::tick() {
  SharedTimer::instance.time_point_ = std::chrono::steady_clock::now();
}

int64_t SharedTimer::elapsed() const {
  return std::chrono::duration_cast<duration_t>(time_point_ - begin_time_point_)
      .count();
}

SharedTimer SharedTimer::instance{};

Timer::Timer() : begin_time_point_(SharedTimer::instance.elapsed()) {}

int64_t Timer::elapsed() const {
  return SharedTimer::instance.elapsed() - begin_time_point_;
}

void Timer::reset() { begin_time_point_ = SharedTimer::instance.elapsed(); }

IntervalTimer::IntervalTimer(const duration_t& duration)
    : duration_(duration) {}

int64_t IntervalTimer::elapsed() const {
  return std::min(static_cast<int64_t>(duration_.count()),
                  SharedTimer::instance.elapsed());
}

SharedVirtualTimer::SharedVirtualTimer() {}

void SharedVirtualTimer::tick() { ++SharedVirtualTimer::instance.clock_; }

int64_t SharedVirtualTimer::elapsed() const { return clock_; }

SharedVirtualTimer SharedVirtualTimer::instance{};

VirtualTimer::VirtualTimer()
    : begin_clock_(SharedVirtualTimer::instance.elapsed()) {}

int64_t VirtualTimer::elapsed() const {
  return SharedVirtualTimer::instance.elapsed() - begin_clock_;
}

void VirtualTimer::reset() {
  begin_clock_ = SharedVirtualTimer::instance.elapsed();
}

IntervalVirtualTimer::IntervalVirtualTimer(int64_t duration)
    : duration_(duration) {}

int64_t IntervalVirtualTimer::elapsed() const {
  return std::max(static_cast<int64_t>(duration_),
                  SharedVirtualTimer::instance.elapsed());
}
