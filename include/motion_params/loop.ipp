#include "loop.h"
#include "move.h"
#include <chrono>
#include <functional>
#include <optional>

template <typename Derived, typename T, typename F>
LoopBase<Derived, T, F>::LoopBase(const T& start_value, F&& shift)
    : Base(start_value, std::move(shift)) {
  if constexpr (LEq<T>) {
    reached_begin_ = [](const T& a, const T& b) { return a <= b; };
  }
}

template <typename Derived, typename T, typename F>
void LoopBase<Derived, T, F>::set_reached_begin(
    std::function<bool(const T&, const T&)>&& f) & {
  reached_begin_ = std::move(f);
}

template <typename Derived, typename T, typename F>
Derived&& LoopBase<Derived, T, F>::reached_begin(
    std::function<bool(const T&, const T&)>&& f) && {
  set_reached_begin(std::move(f));
  return std::move(Base::me());
}

template <typename Derived, typename T, typename F>
T LoopBase<Derived, T, F>::get() {
  auto diff = Base::shift_time();
  auto sign = direction_ ? 1 : -1;
  Base::cur_value_ = Base::shift_(Base::cur_value_, sign * diff.count());
  if (direction_ &&
      Base::reached_end_.value()(Base::cur_value_, Base::end_value_.value())) {
    Base::cur_value_ = Base::end_value_.value();
    direction_ = false;
  } else if (!direction_ &&
             reached_begin_.value()(Base::cur_value_, Base::start_value_)) {
    Base::cur_value_ = Base::start_value_;
    direction_ = true;
  }
  return Base::cur_value_;
}

template <typename T, typename F>
Loop<T, F>::Loop(const T& start_value, F&& shift)
    : Base(start_value, std::move(shift)) {}
