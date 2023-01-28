#pragma once

#include <chrono>
#include <functional>
#include <optional>

template <typename T> class AbstractBase {
public:
  virtual T get() = 0;
};

template <typename Derived, typename T>
class AbstractMove : public AbstractBase<T> {
protected:
  Derived&& me();
};

template <typename Derived, typename T, typename F = void> class MoveBase;
template <typename Derived, typename T> class MoveBase<Derived, T, void>;

template <typename Derived, typename T>
class MoveBase<Derived, T, void> : public AbstractMove<Derived, T> {
  using Base = AbstractMove<Derived, T>;
  using duration_t = std::chrono::duration<long, std::milli>;

public:
  MoveBase(const T& start_value);
  void set_end_value(const T& end) &;
  Derived&& end_value(const T& end) &&;
  void set_duration(const duration_t& d) &;
  Derived&& duration(const duration_t& d) &&;
  void set_reached_end(std::function<bool(const T&, const T&)>&& f) &;
  Derived&& reached_end(std::function<bool(const T&, const T&)>&& f) &&;
  virtual T get() override;

protected:
  duration_t shift_time();

protected:
  std::chrono::time_point<std::chrono::steady_clock> prev_time_;
  std::optional<std::chrono::time_point<std::chrono::steady_clock>> end_time_;
  bool ended_ = false;
  T start_value_;
  T cur_value_;
  std::optional<T> end_value_;
  std::optional<std::function<bool(const T&, const T&)>> reached_end_;
};

template <typename Derived, typename T, typename F>
class MoveBase : public MoveBase<Derived, T, void> {
  using Base = MoveBase<Derived, T, void>;

public:
  MoveBase(const T& start_value, F&& shift);
  virtual T get() override;

protected:
  F shift_;
};

template <typename T, typename F>
class Move final : public MoveBase<Move<T, F>, T, F> {
  using Base = MoveBase<Move<T, F>, T, F>;

public:
  Move(const T& start_value, F&& shift);
};

template <typename T>
concept LEq = requires(T a, T b) {
                { a <= b } -> std::same_as<bool>;
              };

#include "move.ipp"
