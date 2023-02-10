#pragma once

#include "motion_params/timer.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

template <typename T> class AbstractBase {
public:
  AbstractBase() = default;
  virtual ~AbstractBase() {}
  virtual T get() = 0;
};

template <typename Derived, typename T>
class AbstractMove : public AbstractBase<T> {
protected:
  Derived&& me();
};

template <typename Derived, typename T, typename F>
class MoveBase : public AbstractMove<Derived, T> {
  using Base = AbstractMove<Derived, T>;

public:
  MoveBase(const T& start_value, F&& shift,
           std::shared_ptr<AbstractTimer> timer);
  void set_end_value(const T& end) &;
  Derived&& end_value(const T& end) &&;
  void set_reached_end(std::function<bool(const T&, const T&)>&& f) &;
  Derived&& reached_end(std::function<bool(const T&, const T&)>&& f) &&;
  virtual T get() override;

protected:
  T start_value_;
  F shift_;
  std::optional<T> end_value_;
  std::optional<std::function<bool(const T&, const T&)>> reached_end_;
  std::shared_ptr<AbstractTimer> timer_;
};

template <typename T, typename F>
class Move final : public MoveBase<Move<T, F>, T, F> {
  using Base = MoveBase<Move<T, F>, T, F>;

public:
  Move(const T& start_value, F&& shift, std::shared_ptr<AbstractTimer> timer);
};

template <typename T>
concept LEq = requires(T a, T b) {
                { a <= b } -> std::same_as<bool>;
              };

#include "move.ipp"
