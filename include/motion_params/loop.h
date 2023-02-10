#pragma once

#include "move.h"
#include "timer.h"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>

template <typename Derived, typename T, typename F>
struct LoopBase : public MoveBase<Derived, T, F> {
  using Base = MoveBase<Derived, T, F>;

public:
  LoopBase(const T& start_value, F&& shift,
           std::shared_ptr<AbstractTimer> timer);
  void set_reached_begin(std::function<bool(const T&, const T&)>&& f) &;
  Derived&& reached_begin(std::function<bool(const T&, const T&)>&& f) &&;
  virtual T get() override;

private:
  bool direction_ = true;
  std::optional<std::function<bool(const T&, const T&)>> reached_begin_;
};

template <typename T, typename F>
class Loop final : public LoopBase<Loop<T, F>, T, F> {
  using Base = LoopBase<Loop<T, F>, T, F>;

public:
  Loop(const T& start_value, F&& shift, std::shared_ptr<AbstractTimer> timer);
};

#include "loop.ipp"
