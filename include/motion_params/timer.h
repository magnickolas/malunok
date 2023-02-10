#pragma once

#include <chrono>
#include <cstdint>
#include <optional>

class AbstractTimer {
public:
  virtual int64_t elapsed() const = 0;
  virtual void reset() = 0;
  virtual ~AbstractTimer();
};

class SharedTimer {
  using duration_t = std::chrono::duration<long, std::milli>;
  using time_point_t = std::chrono::time_point<std::chrono::steady_clock>;

private:
  SharedTimer();
  SharedTimer(const SharedTimer&) = delete;
  SharedTimer& operator=(const SharedTimer&) = delete;

public:
  static void tick();
  int64_t elapsed() const;

private:
  const time_point_t begin_time_point_;
  time_point_t time_point_;

public:
  static SharedTimer instance;
};

class Timer : public AbstractTimer {
public:
  Timer();
  int64_t elapsed() const override;
  void reset() override;

private:
  int64_t begin_time_point_;
};

class IntervalTimer : public AbstractTimer {
  using duration_t = std::chrono::duration<long, std::milli>;

public:
  IntervalTimer(const duration_t& duration);
  int64_t elapsed() const override;

private:
  duration_t duration_;
};

class SharedVirtualTimer {
private:
  SharedVirtualTimer();
  SharedVirtualTimer(const SharedVirtualTimer&) = delete;
  SharedVirtualTimer& operator=(const SharedVirtualTimer&) = delete;

public:
  static void tick();
  int64_t elapsed() const;

private:
  int64_t clock_ = 0;

public:
  static SharedVirtualTimer instance;
};

class VirtualTimer : public AbstractTimer {
public:
  VirtualTimer();
  int64_t elapsed() const override;
  void reset() override;

private:
  int64_t begin_clock_;
};

class IntervalVirtualTimer : public AbstractTimer {
public:
  IntervalVirtualTimer(int64_t duration);
  int64_t elapsed() const override;

private:
  int64_t duration_;
};
